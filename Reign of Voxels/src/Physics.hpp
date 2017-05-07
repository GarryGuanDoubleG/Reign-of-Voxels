#pragma once

#include <btBulletDynamicsCommon.h>
#include <vector>

#include <GL\glew.h>
#include <SFML\OpenGL.hpp>
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


/**taken from Jacob Maher at 
https://github.com/maher940/IT485_New
**/
class BulletDebugDrawer : public btIDebugDraw 
{
public:
	void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix) {
		glUseProgram(0); // Use Fixed-function pipeline (no shaders)
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(&pViewMatrix[0][0]);
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(&pProjectionMatrix[0][0]);
	}
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
		glColor3f(color.x(), color.y(), color.z());
		glBegin(GL_LINES);
		glVertex3f(from.x(), from.y(), from.z());
		glVertex3f(to.x(), to.y(), to.z());
		glEnd();
	}
	virtual void drawContactPoint(const btVector3 &, const btVector3 &, btScalar, int, const btVector3 &) {}
	virtual void reportErrorWarning(const char *) {}
	virtual void draw3dText(const btVector3 &, const char *) {}
	virtual void setDebugMode(int p) {
		m = p;
	}
	int getDebugMode(void) const { return 3; }
	int m;
};

class Physics
{
public:
	btDynamicsWorld* m_world;/*<world */
	btDispatcher* dispatcher; /*< collison solver*/
	btCollisionConfiguration* collisionConfig; /*< type of collision*/
	btBroadphaseInterface* broadphase; 
	btConstraintSolver* solver; /*< calculates for collisions*/


	std::vector<btRigidBody*>m_rigidBodyList;
	std::vector<btRigidBody*>m_entityBodies;
	std::vector<glm::vec3>bodiesPos;

	CollisionCallback callback;

	Physics();

	void PhysicsStep(float time);
	
	btRigidBody* TriRigidBody(glm::vec3 position, float mass, std::vector<VoxelVertex> &vertices, std::vector<GLuint> &indices);
	btRigidBody* SphereRigidBody(float radius, glm::vec3 position, float mass);
	btRigidBody* CubeRigidBody(glm::vec3 size, glm::vec3 position, float mass);
	btRigidBody* CubeRigidBodyTR(glm::vec3 size, glm::vec3 position, float mass, btQuaternion* quat);
	//btRigidBody* MeshRigidBody(glm::vec3 position, float mass, Mesh mesh);
	btRigidBody* PlaneBody();

	void Physics::deleteRigidBody();
	void Physics::deletePhysicsWorld();
	void Physics::deleteRigidBodyOne(int i);
	void Physics::deleteMelee();

	void CollisionTest(btRigidBody* body);
	void Physics::CollisionTest2(btRigidBody* body1, btRigidBody* body2);
};
