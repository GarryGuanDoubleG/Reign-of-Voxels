#include "Physics.hpp"


Physics::Physics()
{
	int max_entites = 400;

	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);

	m_world->setGravity(btVector3(0, -9.81, 0));

	m_entityBodies.reserve(400);

	for (int i = 0; i < max_entites; i++)
	{
		btTransform t;

		t.setIdentity();
		t.setOrigin(btVector3(0, -8, 0));

		btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btMotionState* motion = new btDefaultMotionState(t);

		btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);

		btRigidBody* body = new btRigidBody(info);
		m_entityBodies.push_back(body);
	}

}


btRigidBody* Physics::TriRigidBody(glm::vec3 position, float mass, std::vector<VoxelVertex> &vertices, std::vector<GLuint> &indices)
{
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0,0,0));

	btTriangleMesh *mTriMesh = new btTriangleMesh();

	int numtri = indices.size() / 3;

	for (int i = 0; i < numtri; i++)
	{
		int j = i * 3;

		btVector3 tri1;

		int tr1 = indices[j];

		tri1.setX(vertices[tr1].position.x);
		tri1.setY(vertices[tr1].position.y);
		tri1.setZ(vertices[tr1].position.z);

		btVector3 tri2;

		int tr2 = indices[j + 1];

		tri2.setX(vertices[tr2].position.x);
		tri2.setY(vertices[tr2].position.y);
		tri2.setZ(vertices[tr2].position.z);

		btVector3 tri3;

		int tr3 = indices[j + 2];

		tri3.setX(vertices[tr3].position.x);
		tri3.setY(vertices[tr3].position.y);
		tri3.setZ(vertices[tr3].position.z);

		mTriMesh->addTriangle(tri1, tri2, tri3);
	}

	btCollisionShape *cube = new btBvhTriangleMeshShape(mTriMesh, true);

	btVector3 inertia(0, 0, 0);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cube, inertia);
	btRigidBody* body = new btRigidBody(info);

	m_world->addRigidBody(body);
	m_rigidBodyList.push_back(body);

	return body;
}

btRigidBody* Physics::CubeRigidBody(glm::vec3 size, glm::vec3 position, float mass)
{
	btTransform t;

	t.setIdentity();
	t.setOrigin(btVector3(position.x, position.y, position.z));

	btVector3 inertia(0, 0, 0);
	btBoxShape *aabb = new btBoxShape(btVector3(size.x, size.y, size.z));

	btMotionState* motion;

	if (mass != 0.0)
	{
		aabb->calculateLocalInertia(mass, inertia);
		motion = new btDefaultMotionState(t);
	}
	else
	{
		motion = NULL;
	}

	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, aabb, inertia);
	btRigidBody* body = new btRigidBody(info);

	m_entityBodies.push_back(body);
	m_world->addRigidBody(body);

	return body;
}