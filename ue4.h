#pragma once

inline float screen_width = GetSystemMetrics(SM_CXSCREEN);
inline float screen_height = GetSystemMetrics(SM_CYSCREEN);

Memory* KmDrv = nullptr;

#define FN_POINTER uintptr_t
#define FN_OFFSET ptrdiff_t
#define FN_VECTOR Vector3

#define M_PI 3.14159265358979323846264338327950288419716939937510



namespace unreal
{
	template<class type>
	class tarray
	{
	public:
		tarray() : data(nullptr), count(std::int32_t()), maxx(std::int32_t()) { }
		tarray(type* data, std::int32_t count, std::int32_t maxx) : data(data), count(count), maxx(maxx) { }

		const bool is_valid() const noexcept
		{
			return !(this->data == nullptr);
		}

		const std::int32_t size() const noexcept
		{
			return this->count;
		}

		type operator[](std::int32_t index) noexcept
		{
			return (type)KmDrv->read<type>((uintptr_t)data + index * sizeof(uintptr_t));
		}

		const type& operator[](std::int32_t index) const noexcept
		{
			return (const type)KmDrv->read<type>((uintptr_t)data + index * sizeof(uintptr_t));
		}

		bool is_valid_index(std::int32_t index) const noexcept
		{
			return index < this->size();
		}

	protected:
		type* data;
		std::int32_t count;
		std::int32_t maxx;
	};

	struct fmatrix
	{
		union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};

		fmatrix multiply(fmatrix other)
		{
			fmatrix pOut;
			pOut._11 = _11 * other._11 + _12 * other._21 + _13 * other._31 + _14 * other._41;
			pOut._12 = _11 * other._12 + _12 * other._22 + _13 * other._32 + _14 * other._42;
			pOut._13 = _11 * other._13 + _12 * other._23 + _13 * other._33 + _14 * other._43;
			pOut._14 = _11 * other._14 + _12 * other._24 + _13 * other._34 + _14 * other._44;
			pOut._21 = _21 * other._11 + _22 * other._21 + _23 * other._31 + _24 * other._41;
			pOut._22 = _21 * other._12 + _22 * other._22 + _23 * other._32 + _24 * other._42;
			pOut._23 = _21 * other._13 + _22 * other._23 + _23 * other._33 + _24 * other._43;
			pOut._24 = _21 * other._14 + _22 * other._24 + _23 * other._34 + _24 * other._44;
			pOut._31 = _31 * other._11 + _32 * other._21 + _33 * other._31 + _34 * other._41;
			pOut._32 = _31 * other._12 + _32 * other._22 + _33 * other._32 + _34 * other._42;
			pOut._33 = _31 * other._13 + _32 * other._23 + _33 * other._33 + _34 * other._43;
			pOut._34 = _31 * other._14 + _32 * other._24 + _33 * other._34 + _34 * other._44;
			pOut._41 = _41 * other._11 + _42 * other._21 + _43 * other._31 + _44 * other._41;
			pOut._42 = _41 * other._12 + _42 * other._22 + _43 * other._32 + _44 * other._42;
			pOut._43 = _41 * other._13 + _42 * other._23 + _43 * other._33 + _44 * other._43;
			pOut._44 = _41 * other._14 + _42 * other._24 + _43 * other._34 + _44 * other._44;
			return pOut;
		}
	};

	struct fvector
	{
		double X;
		double Y;
		double Z;

		inline fvector()
			: X(0), Y(0), Z(0)
		{ }

		inline fvector(float x, float y, float z)
			: X(x),
			Y(y),
			Z(z)
		{ }

		fvector operator-(fvector v)
		{
			return fvector(X - v.X, Y - v.Y, Z - v.Z);
		}

		fvector operator+(fvector v)
		{
			return fvector(X + v.X, Y + v.Y, Z + v.Z);
		}

		fvector operator*(float s)
		{
			return fvector(X * s, Y * s, Z * s);
		}

		float distance(fvector v)
		{
			return ((X - v.X) * (X - v.X) +
				(Y - v.Y) * (Y - v.Y) +
				(Z - v.Z) * (Z - v.Z));
		}

		float dot(fvector v)
		{
			return X * v.X + Y * v.Y + Z * v.Z;
		}
	};

	struct fvector2d
	{
		double X;
		double Y;

		inline fvector2d()
			: X(0), Y(0)
		{ }

		inline fvector2d(float x, float y)
			: X(x),
			Y(y)
		{ }

	};

	struct frotator
	{
		double                                              Pitch;                                                        // 0x0000(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		double                                              Yaw;                                                        // 0x0008(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
		double                                              Roll;                                                        // 0x0010(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)

		inline frotator()
			: Pitch(0), Yaw(0), Roll(0)
		{ }

		inline frotator(double pitch, double yaw, double roll)
			: Pitch(pitch),
			Yaw(yaw),
			Roll(roll)
		{ }

		fmatrix Matrix(frotator origin = { 0, 0, 0 })
		{
			float radPitch = (Pitch * double(3.14159265358979323846264338327950288419716939937510) / 180.f);
			float radYaw = (Yaw * double(3.14159265358979323846264338327950288419716939937510) / 180.f);
			float radRoll = (Roll * double(3.14159265358979323846264338327950288419716939937510) / 180.f);

			float SP = sinf(radPitch);
			float CP = cosf(radPitch);
			float SY = sinf(radYaw);
			float CY = cosf(radYaw);
			float SR = sinf(radRoll);
			float CR = cosf(radRoll);

			fmatrix matrix;
			matrix.m[0][0] = CP * CY;
			matrix.m[0][1] = CP * SY;
			matrix.m[0][2] = SP;
			matrix.m[0][3] = 0.f;

			matrix.m[1][0] = SR * SP * CY - CR * SY;
			matrix.m[1][1] = SR * SP * SY + CR * CY;
			matrix.m[1][2] = -SR * CP;
			matrix.m[1][3] = 0.f;

			matrix.m[2][0] = -(CR * SP * CY + SR * SY);
			matrix.m[2][1] = CY * SR - CR * SP * SY;
			matrix.m[2][2] = CR * CP;
			matrix.m[2][3] = 0.f;

			matrix.m[3][0] = origin.Pitch;
			matrix.m[3][1] = origin.Yaw;
			matrix.m[3][2] = origin.Roll;
			matrix.m[3][3] = 1.f;

			return matrix;
		}
	};

	struct fquat
	{
		double x;
		double y;
		double z;
		double w;
	};

	struct ftransform
	{
		fquat rot;
		fvector translation;
		char pad[0x4];
		fvector scale;
		char pad1[0x4];

		fmatrix to_matrix_with_scale()
		{
			fmatrix m;
			m._41 = translation.X;
			m._42 = translation.Y;
			m._43 = translation.Z;

			double x2 = rot.x + rot.x;
			double y2 = rot.y + rot.y;
			double z2 = rot.z + rot.z;

			double xx2 = rot.x * x2;
			double yy2 = rot.y * y2;
			double zz2 = rot.z * z2;
			m._11 = (1.0f - (yy2 + zz2)) * scale.X;
			m._22 = (1.0f - (xx2 + zz2)) * scale.Y;
			m._33 = (1.0f - (xx2 + yy2)) * scale.Z;

			double yz2 = rot.y * z2;
			double wx2 = rot.w * x2;
			m._32 = (yz2 - wx2) * scale.Z;
			m._23 = (yz2 + wx2) * scale.Y;

			double xy2 = rot.x * y2;
			double wz2 = rot.w * z2;
			m._21 = (xy2 - wz2) * scale.Y;
			m._12 = (xy2 + wz2) * scale.X;

			double xz2 = rot.x * z2;
			double wy2 = rot.w * y2;
			m._31 = (xz2 + wy2) * scale.Z;
			m._13 = (xz2 - wy2) * scale.X;

			m._14 = 0.0f;
			m._24 = 0.0f;
			m._34 = 0.0f;
			m._44 = 1.0f;

			return m;
		}
	};

	


	namespace defines
	{
		uintptr_t gworld;
		fvector camera_location;
		frotator camera_rotation;
		float fov_angle;
	}

	namespace functions
	{
		void update_camera(uintptr_t world, uintptr_t local_player, uintptr_t root_component)
		{
			auto camera_chain = KmDrv->read<uintptr_t>(KmDrv->read<uintptr_t>(local_player + 0xd0) + 0x8);
			if (!camera_chain) return;

			unreal::defines::camera_location = KmDrv->read<unreal::fvector>(KmDrv->read<uintptr_t>(world + 0x110));
			//utils::debug_log("unreal::defines::camera_location ->  x: %f, y: %f, z: %f", unreal::defines::camera_location.X, unreal::defines::camera_location.Y, unreal::defines::camera_location.Z);

			unreal::defines::camera_rotation =
			{
				asin(KmDrv->read<double>(camera_chain + 0x9C0)) * (180.f / 3.14159265358979323846264338327950288419716939937510),
				KmDrv->read<double>(root_component + 0x148),
				0
			};
			//utils::debug_log("unreal::defines::camera_rotation ->  x: %f, y: %f, z: %f", unreal::defines::camera_rotation.Pitch, unreal::defines::camera_rotation.Yaw, unreal::defines::camera_rotation.Roll);

			
			unreal::defines::fov_angle = 80.0f / (KmDrv->read<double>(camera_chain + 0x7F0) / 1.19f);
			//utils::debug_log("unreal::defines::fov_angle ->  %f", unreal::defines::fov_angle);
		}

		fvector get_actor_location(uintptr_t actor)
		{
			auto root_component = KmDrv->read<uintptr_t>(actor + 0x190);
			if (!root_component) return {};

			return KmDrv->read<fvector>(root_component + 0x128);
		}



		fvector get_bone_location(uintptr_t mesh, int index)
		{
			uintptr_t bone_array = KmDrv->read<uintptr_t>(mesh + 0x5E8);

			if (!bone_array)
				bone_array = KmDrv->read<uintptr_t>(mesh + 0x5E8 + 0x10);

			if (!bone_array)
				return {};

			auto bone = KmDrv->read<ftransform>(bone_array + (index * 0x60));

			auto component_to_world = KmDrv->read<ftransform>(mesh + 0x240);

			auto ret = bone.to_matrix_with_scale().multiply(component_to_world.to_matrix_with_scale());

			return { ret._41, ret._42, ret._43 };
		}

		ImVec2 world_to_screen(fvector world_location)
		{
			defines::camera_rotation.Roll = 0;

			if (defines::camera_rotation.Yaw < 0)
				defines::camera_rotation.Yaw += 360;

			fmatrix matrix(defines::camera_rotation.Matrix());

			auto dist = world_location - defines::camera_location;

			auto transform = fvector(
				dist.dot(fvector(
					matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]
				)),
				dist.dot(fvector(
					matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]
				)),
				dist.dot(fvector(
					matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]
				))
			);

			if (transform.Z < 1.f)
				transform.Z = 1.f;


			float ScreenCenterX = screen_width / 2;
			float ScreenCenterY = screen_height / 2;

			ImVec2 Screenlocation;
			Screenlocation.x = ScreenCenterX + transform.X * (ScreenCenterX / tanf(defines::fov_angle * utils::pigreek / 360.f)) / transform.Z;
			Screenlocation.y = ScreenCenterY - transform.Y * (ScreenCenterX / tanf(defines::fov_angle * utils::pigreek / 360.f)) / transform.Z;

			return Screenlocation;
		}
	}
}