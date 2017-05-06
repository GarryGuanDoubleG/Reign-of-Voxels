#pragma once

#include <btBulletDynamicsCommon.h>
#include <vector>
#include "3dmath.hpp"

//struct testCallback : public btCollisionWorld::ContactResultCallback
//{
//
//	testCallback(Test_Struct* context) : ctext(context) {}
//
//	virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, 
//									int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int 
//									partId1, int index1)
//	{
//
//
//		//ctext += 10;
//		ctext->points += 10;
//		//slog("points %f", ctext->points);
//		return 0;
//	}
//
//	Test_Struct* ctext;
//};


struct CollisionCallback : public btCollisionWorld::ContactResultCallback
{
	virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
	{
		return 0;
	}
};

class Physics
{
public:
	btDynamicsWorld* space;/*<world */
	btDispatcher* dispatcher; /*< collison solver*/
	btCollisionConfiguration* collisionConfig; /*< type of collision*/
	btBroadphaseInterface* broadphase; 
	btConstraintSolver* solver; /*< calculates for collisions*/

	std::vector<btRigidBody*>bodies;
	std::vector<btRigidBody*>Melee;
	std::vector<btRigidBody*>Bullets;
	std::vector<glm::vec3>bodiesPos;

	CollisionCallback callback;

	Physics();

	void PhysicsStep(float time);

	btRigidBody* SphereRigidBody(float radius, glm::vec3 position, float mass);
	btRigidBody* CubeRigidBody(glm::vec3 size, glm::vec3 position, float mass);
	btRigidBody* CubeRigidBodyTR(glm::vec3 size, glm::vec3 position, float mass, btQuaternion* quat);
	btRigidBody* MeshRigidBody(glm::vec3 position, float mass, Mesh mesh);
	btRigidBody* PlaneBody();

	void Physics::deleteRigidBody();
	void Physics::deletePhysicsWorld();
	void Physics::deleteRigidBodyOne(int i);
	void Physics::deleteMelee();

	void CollisionTest(btRigidBody* body);
	void Physics::CollisionTest2(btRigidBody* body1, btRigidBody* body2);
};
