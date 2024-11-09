#include <string.h>
#include <list>
#include <string>
#include "ue.hpp"

struct Camera
{
	fvector location;
	fvector rotation;
	float fov;
};

namespace cache
{
	inline uintptr_t uworld;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;
	inline uintptr_t currentweapon;
	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline int my_team_id;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline fvector relative_location;
	inline int player_count;
	inline float closest_distance;
	inline uintptr_t closest_mesh;
	inline  camera_position_s local_camera;
}

struct FNRot
{
	double a;
	char pad_0008[24];
	double b;
	char pad_0028[424];
	double c;
};

DWORD_PTR Uworld_Cam;

namespace uee
{
	class ue 
	{
	public:

		auto get_player_name(uintptr_t player_state) -> std::string
		{
			auto fstring = read<__int64>(player_state + 0xA98);
			auto length = read<int>(fstring + 16);

			auto v6 = (__int64)length;

			if (!v6) return std::string(verucryptt("In Lobby"));

			auto IsABot = read<char>(player_state + 0x29A & 0x8);

			if (IsABot) return std::string(verucryptt("AI"));

			auto ftext = (uintptr_t)read<__int64>(fstring + 8);

			wchar_t* buffer = new wchar_t[length];
			/*Frozen->FrozenRead(static_cast<ULONGLONG>(ftext), buffer, length * sizeof(wchar_t));*/

			char v21;
			int v22;
			int i;

			int v25;
			UINT16* v23;

			v21 = v6 - 1;
			if (!(UINT32)v6)
				v21 = 0;
			v22 = 0;
			v23 = (UINT16*)buffer;
			for (i = (v21) & 3; ; *v23++ += i & 7)
			{
				v25 = v6 - 1;
				if (!(UINT32)v6)
					v25 = 0;
				if (v22 >= v25)
					break;
				i += 3;
				++v22;
			}

			std::wstring username{ buffer };
			delete[] buffer;
			return std::string(username.begin(), username.end());
		}
		//Not updated Name dec

		auto in_screen(fvector2d screen_location) -> bool {

			if (screen_location.x > 0 && screen_location.x < screen_width && screen_location.y > 0 && screen_location.y < screen_height) return true;
			else return false;
		}


		camera_position_s get_camera()
		{
			camera_position_s view_point{};
			uintptr_t location_pointer = read<uintptr_t>(arrays->uworld + 0x120);
			uintptr_t rotation_pointer = read<uintptr_t>(arrays->uworld + 0x130);
			FNRot fnrot{};
			fnrot.a = read<double>(rotation_pointer);
			fnrot.b = read<double>(rotation_pointer + 0x20);
			fnrot.c = read<double>(rotation_pointer + 0x1D0);
			view_point.location = read<fvector>(location_pointer);
			view_point.rotation.x = asin(fnrot.c) * (180.0 / M_PI);
			view_point.rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
			view_point.fov = read<float>(arrays->player_controller + 0x3AC) * 90.f;
			return view_point;
		}

		void RadarRange(float* x, float* y, float range)
		{
			if (fabs((*x)) > range || fabs((*y)) > range)
			{
				if ((*y) > (*x))
				{
					if ((*y) > -(*x))
					{
						(*x) = range * (*x) / (*y);
						(*y) = range;
					}
					else
					{
						(*y) = -range * (*y) / (*x);
						(*x) = -range;
					}
				}
				else
				{
					if ((*y) > -(*x))
					{
						(*y) = range * (*y) / (*x);
						(*x) = range;
					}
					else
					{
						(*x) = -range * (*x) / (*y);
						(*y) = -range;
					}
				}
			}
		}


		inline fvector2d w2s(fvector WorldLocation)
		{
			cache::local_camera = get_camera();
			D3DMATRIX temp_matrix = to_matrix(cache::local_camera.rotation);
			fvector vaxisx = fvector(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
			fvector vaxisy = fvector(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
			fvector vaxisz = fvector(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
			fvector vdelta = WorldLocation - cache::local_camera.location;
			fvector vtransformed = fvector(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
			if (vtransformed.z < 1) vtransformed.z = 1;

			// Compute screen coordinates based on transformed coordinates and FOV
			return fvector2d(
				(screen_width/ 2.0f) + vtransformed.x * (((screen_width / 2.0f) / tanf(cache::local_camera.fov * (float)M_PI / 360.f))) / vtransformed.z,
				(screen_height / 2.0f) - vtransformed.y * (((screen_width / 2.0f) / tanf(cache::local_camera.fov * (float)M_PI / 360.f))) / vtransformed.z
			);
		}

		static auto Bone(uintptr_t skeletal_mesh, int bone_index) -> fvector {
			uintptr_t bone_array = read<uintptr_t>(skeletal_mesh + offset::bone_array);
			if (bone_array == NULL) bone_array = read<uintptr_t>(skeletal_mesh + offset::bone_array + 0x10);
			FTransform bone = read<FTransform>(bone_array + (bone_index * 0x60));
			FTransform comp2world = read<FTransform>(skeletal_mesh + offset::component_to_world);
			D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), comp2world.ToMatrixWithScale());
			return fvector(matrix._41, matrix._42, matrix._43);
		}

		
		auto IsShootable(fvector lur, fvector wl) -> bool {

			if (lur.x >= wl.x - 20 && lur.x <= wl.x + 20 && lur.y >= wl.y - 20 && lur.y <= wl.y + 20 && lur.z >= wl.z - 30 && lur.z <= wl.z + 30)
				return true;
			else
				return false;

		}


		static auto is_visible(uintptr_t skeletal_mesh) -> bool {

			auto last_submit = read<float>(skeletal_mesh + 0x2EC);
			auto last_render = read<float>(skeletal_mesh + 0x2F0);
			return (bool)(last_render + 0.06f >= last_submit);
		}

		fvector Prediction(fvector TargetPosition, fvector ComponentVelocity, float player_distance, float ProjectileSpeed = 239)
		{
			float gravity = 3.5;
			float TimeToTarget = player_distance / ProjectileSpeed;
			float bulletDrop = abs(gravity) * (TimeToTarget * TimeToTarget) * 0.5;
			return fvector
			{
			TargetPosition.x + TimeToTarget * ComponentVelocity.x,
			TargetPosition.y + TimeToTarget * ComponentVelocity.y,
			TargetPosition.z + TimeToTarget * ComponentVelocity.z + bulletDrop
			};
		}




		__forceinline auto skeleton(uintptr_t mesh) -> void
		{
			const int color = ImColor(globals::g_color[0], globals::g_color[1], globals::g_color[2]);



			fvector bonePositions[] = {
				Bone(mesh, 109),  // HeadBone
				Bone(mesh, 2),   // Hip
				Bone(mesh, 66),  // Neck
				Bone(mesh, 9),   // UpperArmLeft
				Bone(mesh, 38),  // UpperArmRight
				Bone(mesh, 10),  // LeftHand
				Bone(mesh, 39),  // RightHand
				Bone(mesh, 11),  // LeftHand1
				Bone(mesh, 40),  // RightHand1
				Bone(mesh, 78),  // RightThigh
				Bone(mesh, 71),  // LeftThigh
				Bone(mesh, 79),  // RightCalf
				Bone(mesh, 72),  // LeftCalf
				Bone(mesh, 75),  // LeftFoot 
				Bone(mesh, 82)   // RightFoot
			};

			fvector2d bonePositionsOut[16];
			for (int i = 0; i < 16; ++i) {
				bonePositionsOut[i] = w2s(bonePositions[i]);
			}

			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), ImVec2(bonePositionsOut[2].x, bonePositionsOut[2].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[3].x, bonePositionsOut[3].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[4].x, bonePositionsOut[4].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[5].x, bonePositionsOut[5].y), ImVec2(bonePositionsOut[7].x, bonePositionsOut[7].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[6].x, bonePositionsOut[6].y), ImVec2(bonePositionsOut[8].x, bonePositionsOut[8].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), ImVec2(bonePositionsOut[1].x, bonePositionsOut[1].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), ImVec2(bonePositionsOut[10].x, bonePositionsOut[10].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), ImVec2(bonePositionsOut[9].x, bonePositionsOut[9].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[13].x, bonePositionsOut[13].y), ImVec2(bonePositionsOut[12].x, bonePositionsOut[12].y), color, globals::g_skeletonthickness);
			ImGui::GetBackgroundDrawList()->AddLine(ImVec2(bonePositionsOut[14].x, bonePositionsOut[14].y), ImVec2(bonePositionsOut[11].x, bonePositionsOut[11].y), color, globals::g_skeletonthickness);
		}


	};  static uee::ue* ue5 = new uee::ue();
}


