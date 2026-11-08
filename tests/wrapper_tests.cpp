#include "ofxBox3dAll.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

bool near(double a, double b, double tolerance = 1e-4) {
    return std::abs(a - b) < tolerance;
}

class RenderBox : public ofxBox3dBox {
public:
    using ofxBox3dBody::getRenderTransformMatrix;
};

void lifetimeTests() {
    ofxBox3dBox survivor;
    b3WorldId originalWorld;
    {
        ofxBox3d world;
        world.setup();
        originalWorld = world.getWorldId();
        survivor = world.createBox({0, 5, 0}, {1, 1, 1});
        require(survivor.isValid(), "body should be valid while world lives");
    }
    require(!survivor.isValid(), "body must be invalid after world destruction");
    {
        ofxBox3d replacement;
        replacement.setup();
        auto body = replacement.createSphere({0, 3, 0}, 1);
        require(replacement.getWorldId().index1 == originalWorld.index1, "test should reuse world slot");
        survivor.destroy();
        require(body.isValid() && replacement.getBodyCount() == 1,
                "old body must not destroy replacement world's body");
    }

    ofxBox3d world;
    auto stale = world.createSphere({0, 5, 0}, 1);
    world.clear();
    auto fresh = world.createCapsule({0, 5, 0}, 0.5f, 2);
    require(!stale.isValid(), "body must remain invalid after world reset");
    stale.destroy();
    require(fresh.isValid(), "destroying stale body must leave fresh body intact");
    b3DestroyBody(fresh.getBodyId());
    fresh.destroy();
    require(world.getBodyCount() == 0, "raw body destruction must be safe");
    b3DestroyWorld(world.getWorldId());
    require(!world.isSetup(), "raw world destruction must be detected");
    world.clear();
}

void factoryAndSettingsTests() {
    ofxBox3d world;
    int userData = 7;
    ofxBox3dBodySettings settings;
    settings.density = 2.f;
    settings.friction = 0.7f;
    settings.restitution = 0.4f;
    settings.linearDamping = 0.2f;
    settings.angularDamping = 0.3f;
    settings.gravityScale = 0.5f;
    settings.enableSleep = false;
    settings.isBullet = true;
    settings.userData = &userData;
    settings.enableContactEvents = true;
    settings.enableHitEvents = true;
    settings.enableSensorEvents = true;
    auto box = world.createBox({0, 5, 0}, {1, 2, 3}, settings);
    require(world.isSetup() && box.isValid(), "factory should initialize the world");
    require(world.getBodies().empty(), "value factories should not register shared ownership");
    require(near(box.getMass(), 96), "factory should apply density to shape mass");
    require(near(box.getLinearDamping(), 0.2), "linear damping should be set");
    require(near(box.getAngularDamping(), 0.3), "angular damping should be set");
    require(near(box.getGravityScale(), 0.5), "gravity scale should be set");
    require(box.isBullet(), "bullet setting should be set");
    require(!b3Body_IsSleepEnabled(box.getBodyId()), "sleep setting should be set");
    require(b3Body_GetUserData(box.getBodyId()) == &userData, "user data should be preserved");
    b3ShapeId shape;
    require(b3Body_GetShapes(box.getBodyId(), &shape, 1) == 1, "box should have one shape");
    require(near(b3Shape_GetDensity(shape), settings.density), "density should be set");
    require(near(b3Shape_GetFriction(shape), settings.friction), "friction should be set");
    require(near(b3Shape_GetRestitution(shape), settings.restitution), "restitution should be set");
    require(b3Shape_AreContactEventsEnabled(shape), "contact events should be enabled");
    require(b3Shape_AreHitEventsEnabled(shape), "hit events should be enabled");
    require(b3Shape_AreSensorEventsEnabled(shape), "sensor events should be enabled");
    auto ground = world.createStaticBox({0, -1, 0}, {10, 1, 10}, 0.6f);
    require(ground.getType() == b3_staticBody, "static factory should create static body");
    settings.isSensor = true;
    auto sensor = world.createSphere({0, 10, 0}, 1, settings);
    b3Body_GetShapes(sensor.getBodyId(), &shape, 1);
    require(b3Shape_IsSensor(shape), "sensor setting should be set");
    auto capsule = world.createCapsule({0, 15, 0}, 0.5f, 2);
    require(capsule.isValid() && near(capsule.getHeight(), 2), "capsule factory should preserve dimensions");
    require(world.getBodyCount() == 4, "body count should include unregistered value bodies");
}

template<typename Body, typename Factory>
void moveShapeTest(Factory factory) {
    static_assert(!std::is_copy_constructible<Body>::value, "bodies must not be copied");
    static_assert(std::is_move_constructible<Body>::value, "bodies must be movable");
    static_assert(std::is_move_assignable<Body>::value, "bodies must be move assignable");
    ofxBox3d world;
    Body original = factory(world);
    original.setColor(ofColor::red);
    original.setFilled(false);
    original.ofNode::setScale(2.f);
    ofNode parent;
    original.setParent(parent);
    const auto id = original.getBodyId();
    Body moved(std::move(original));
    require(!original.isValid() && moved.isValid(), "move constructor should transfer ownership");
    require(moved.getBodyId().index1 == id.index1, "move should keep original body ID");
    require(moved.getParent() == &parent && original.getParent() == nullptr, "parent link should transfer");
    require(moved.getColor() == ofColor::red && !moved.isFilled(), "style should transfer");
    require(near(moved.ofNode::getScale().x, 2), "node scale should transfer");
    Body destination = factory(world);
    destination = std::move(moved);
    require(!moved.isValid() && destination.isValid(), "move assignment should transfer ownership");
    require(world.getBodyCount() == 1, "move assignment should release old destination body");
    destination = std::move(destination);
    require(destination.isValid(), "self move must preserve the body");
    original.destroy();
    moved.destroy();
    require(destination.isValid(), "moved-from destruction must not destroy new owner");
}

void moveTests() {
    moveShapeTest<ofxBox3dBox>([](ofxBox3d& w) { return w.createBox({0, 5, 0}, {1, 2, 3}); });
    moveShapeTest<ofxBox3dSphere>([](ofxBox3d& w) { return w.createSphere({0, 5, 0}, 1); });
    moveShapeTest<ofxBox3dCapsule>([](ofxBox3d& w) { return w.createCapsule({0, 5, 0}, 0.5f, 2); });
    ofxBox3d world;
    std::vector<ofxBox3dBox> boxes;
    for (int i = 0; i < 64; ++i) boxes.push_back(world.createBox({float(i * 3), 5, 0}, {1, 1, 1}));
    require(world.getBodyCount() == 64, "vector relocation must preserve all bodies");
    boxes.erase(boxes.begin());
    require(world.getBodyCount() == 63, "vector erase should destroy exactly one body");
    boxes.clear();
    require(world.getBodyCount() == 0, "vector clear should release all bodies");
}

void positionAndRenderTests() {
    ofxBox3dUtils::setScale(100.f);
    ofxBox3d world;
    world.setup({0, -981, 0});
    RenderBox box;
    box.setup(world, {100, 500, 300}, {50, 50, 50});
    world.update(1.f / 60.f);
    require(box.getPosition().y < 500, "value-owned body should simulate without registration");
    auto transform = box.getRenderTransformMatrix();
    require(near(transform[3].y, box.getPosition().y), "render transform should read current physics position");
    ofNode parent;
    parent.setPosition(10, 20, 30);
    box.setParent(parent);
    transform = box.getRenderTransformMatrix();
    require(near(transform[3].x, box.getPosition().x + 10), "render transform should preserve parent integration");
    const glm::dvec3 precise{100000000.125, 500.25, -100000000.375};
    box.setPositionDouble(precise);
    const auto actual = box.getPositionDouble();
    require(near(actual.y, precise.y), "position conversion should respect world scale");
#if defined(BOX3D_DOUBLE_PRECISION)
    require(near(actual.x, precise.x, 1e-7) && near(actual.z, precise.z, 1e-7),
            "double precision position must not pass through float vectors");
#endif
    box.setPosition({100, 200, 300});
    box.setTransform({200, 300, 400}, ofQuaternion());
    box.syncToPhysics();
    box.syncFromPhysics();
    require(near(box.getPosition().x, 200), "legacy transforms should still work");
    ofxBox3dUtils::applyBodyTransform(parent, box.getBodyId());
    ofxBox3dUtils::getBodyMatrix(box.getBodyId());
    ofxBox3dUtils::setScale(1.f);
}
}

int main() {
    try {
        ofxBox3dUtils::setScale(1.f);
        lifetimeTests();
        factoryAndSettingsTests();
        moveTests();
        positionAndRenderTests();
        std::cout << "All ofxBox3d wrapper tests passed (double precision: "
                  << b3IsDoublePrecision() << ")\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
