#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB3.hpp"

void AddVertsForColoredCube3D(std::vector<Vertex_PCU>& verts)
{
	// p-positive n-negative
	Vec3 nnn(-0.5f, -0.5f, -0.5f);
	Vec3 nnp(-0.5f, -0.5f, +0.5f);
	Vec3 npn(-0.5f, +0.5f, -0.5f);
	Vec3 npp(-0.5f, +0.5f, +0.5f);
	Vec3 pnn(+0.5f, -0.5f, -0.5f);
	Vec3 pnp(+0.5f, -0.5f, +0.5f);
	Vec3 ppn(+0.5f, +0.5f, -0.5f);
	Vec3 ppp(+0.5f, +0.5f, +0.5f);

	AddVertsForQuad3D(verts, pnn, ppn, ppp, pnp, Rgba8::RED); // +x
	AddVertsForQuad3D(verts, npn, nnn, nnp, npp, Rgba8::CYAN); // -x
	AddVertsForQuad3D(verts, ppn, npn, npp, ppp, Rgba8::GREEN); // +y
	AddVertsForQuad3D(verts, nnn, pnn, pnp, nnp, Rgba8::MAGENTA); // -y
	AddVertsForQuad3D(verts, npp, nnp, pnp, ppp, Rgba8::BLUE); // +z
	AddVertsForQuad3D(verts, ppn, pnn, nnn, npn, Rgba8::YELLOW); // -z
}

