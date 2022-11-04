#pragma once
#include "pch.h"

#define PI	 3.14159f
#define RAD 2*PI //2 pi = 1 rad

namespace dxh
{
	//Custom variable types


	typedef struct Float2
	{
		float x;
		float y;
		Float2()noexcept : x(0.0f), y(0.0f) {}
		Float2(const Float2& param)noexcept : x(param.x), y(param.y) {}
		Float2& operator=(Float2&& param)noexcept { x = param.x, y = param.y; return *this; }
		Float2& operator=(const Float2& param) { x = param.x, y = param.y; return *this; }
		Float2(const float& _x, const float& _y)noexcept { x = _x, y = _y; }
		//Float2(const float* pArray)noexcept { if (!(pArray == nullptr)) { x = pArray[0], y = pArray[1]; } else { Float2(0.0f, 0.0f); } }
		constexpr float operator[](const size_t _i) { if (_i == 0) return x; return y; }
		bool operator==(const Float2& comp) { return (this->x == comp.x && this->y == comp.y); }
		bool operator!=(const Float2& comp) { return !Float2::operator==(comp); }
		Float2 operator+(const Float2& right)const { return Float2(this->x + right.x, this->y + right.y); }
		Float2 operator-(const Float2& right)const { return Float2(this->x - right.x, this->y - right.y); }
		friend std::ostream& operator<<(std::ostream& os, const Float2& obj)
		{
			os << obj.x << ", " << obj.y;
			return os;
		}
	}float2;

	typedef struct Float3
	{
		float x;
		float y;
		float z;
		Float3()noexcept : x(0.0f), y(0.0f), z(0.0f) { }
		Float3(const Float3& param) :x(param.x), y(param.y), z(param.z) { }
		Float3& operator=(Float3&& param)noexcept { x = param.x, y = param.y, z = param.z; return *this; }
		Float3& operator=(const Float3& param) { x = param.x, y = param.y, z = param.z; return *this; }
		Float3(float _x, float _y, float _z)noexcept { x = _x, y = _y, z = _z; }
		//Float3(const float *pArray)noexcept { if (pArray != NULL) { x = pArray[0], y = pArray[1], z = pArray[2]; } }
		const float& operator[](const size_t _i)const { if (_i == 0) return x; if (_i == 1)return y; return z; }
		constexpr float operator[](const size_t _i) { if (_i == 0) return x; if (_i == 1)return y; return z; }
		bool operator==(const Float3& comp) { return (this->x == comp.x && this->y == comp.y && this->z == comp.z); }
		bool operator!=(const Float3& comp) { return !Float3::operator==(comp); }
		Float3 operator+(const Float3& right)const { return Float3(this->x + right.x, this->y + right.y, this->z + right.z); }
		Float3 operator-(const Float3& right)const { return Float3(this->x - right.x, this->y - right.y, this->z - right.z); }
		Float3 operator*(const float & right)const { return Float3(x * right, y * right, z * right); }
		//For output operations
		friend std::ostream& operator<<(std::ostream& os, const Float3& obj)
		{
			os << obj.x << ", " << obj.y << ", " << obj.z;
			return os;
		}
	}float3;

	typedef struct Float4
	{
		//Variables
		float x;
		float y;
		float z;
		float w;

		Float4()noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
		Float4(const Float4& param) : x(param.x), y(param.y), z(param.z), w(param.w) { }
		Float4& operator=(Float4& param)noexcept { if (!(&param != this)) { x = param.x, y = param.y, z = param.z, w = param.w; } return *this; }
		Float4& operator=(Float4&& param)noexcept { x = std::move(param.x), y = std::move(param.y), z = std::move(param.z), w = std::move(param.w); return *this; }
		Float4(float _x, float _y, float _z, float _w)noexcept :x(_x), y(_y), z(_z), w(_w) { }
		//Float4(const float *pArray) noexcept { if (!(pArray == nullptr)) { x = pArray[0], y = pArray[1], z = pArray[2], w = pArray[3]; } }

		constexpr float operator[](const size_t _i) const { if (_i == 0) return x; if (_i == 1)return y; if (_i == 2) return z; return w; }
		bool operator==(const Float4& comp)const { return (this->x == comp.x && this->y == comp.y && this->z == comp.z && this->w == comp.w); }
		bool operator!=(const Float4& comp)const { return !(Float4::operator==(comp)); }
		Float4 operator+(const Float4& right)const { return Float4(this->x + right.x, this->y + right.y, this->z + right.z, this->w + right.w); }
		Float4 operator-(const Float4& right)const { return Float4(this->x - right.x, this->y - right.y, this->z - right.z, this->w - right.w); }


	}float4;

	//VERTEX ==============================================================================================================================
	typedef struct Vertex
	{
		float3 pos;
		float3 normal;
		float2 uv;
		Vertex()noexcept : pos(0.0f, 0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f), uv(0.0f, 0.0f) { }
		Vertex(const float3& _pos, const float3& _normal, const float2& _uv)
			: pos(_pos), normal(_normal), uv(_uv) {}
		Vertex& operator=(const Vertex& copy) { if (this != &copy) { pos = copy.pos; normal = copy.normal; uv = copy.uv; } return *this; }
		bool operator==(const Vertex& comp) { return (this->pos == comp.pos && pos == comp.pos && uv == comp.uv); }
		bool operator!=(const Vertex& comp) { return !(this == &comp); }
	}vertex;

	using FacePoint = std::vector<size_t>;
	using Face = std::vector<std::vector<size_t>>;

	//FORMULAS ========================================================================================================================
	inline
	const float3 cross(const Float3 AB, const Float3 AC)noexcept
	{
		float3 normal{};
		normal.x = AB.y * AC.z - AB.z * AC.y;
		normal.y = AB.x * AC.z - AB.z * AC.x;
		normal.z = AB.x * AC.y - AB.y * AC.x;
		return normal;
	}
	//MESH ==============================================================================================================================
	class Mesh
	{
	private:
		//class variables
		void AssembleVertices() {
			int i = 0;
			for (const auto& a : faces)
			{
				for (const auto& b : a)
				{
					dxh::Vertex tempvtx;
					//position 0 in a face vector is position, check if it was read
					if (b[0] > 0)
					{
						tempvtx.pos = vpos[b[0] - 1];
					}
					//position 1 is uv coordinate
					if (b[1] > 0)
					{
						tempvtx.uv = vt[b[1] - 1];
					}
					//position 2 is normal 
					if (b[2] > 0)
					{
						tempvtx.normal = vnorm[b[2] - 1];
					}
					vertices.push_back(tempvtx);
					indices.push_back(i++);
				}
			}
		}
	public:
		std::string name;
		std::vector<dxh::Vertex> vertices;
		std::vector<size_t> indices;
		std::vector<Float3> vpos;
		std::vector<Float2> vt;
		std::vector<Float3> vnorm;
		std::vector<Face> faces;

		Mesh() { vpos.clear(); vt.clear(); vnorm.clear(); faces.clear(); name = "unnamed"; }
		Mesh(std::vector<dxh::Float3> _vpos, std::vector<Float3> _vnorm, std::vector<Float2> _vt, std::vector<dxh::Face> _faces)
			: vpos(_vpos), vt(_vt), vnorm(_vnorm), faces(_faces) {}
		Mesh(std::vector<dxh::Vertex> _vtxs, std::vector<size_t> _indices, std::string _name = "unnamed") :vertices(_vtxs), indices(_indices), name(_name) {}

		Mesh(const Mesh& copy) :name(copy.name), vertices(copy.vertices), faces(copy.faces) { }
		~Mesh() {}
		const std::vector<dxh::Vertex>& GetVertices() {
			//check if vertices have been read
			if (!(vertices.size() > 0))
				AssembleVertices();
			return vertices;
		}
		Mesh& operator=(const Mesh& right) {
			if (&right != this) {
				name = right.name;
				vertices = right.vertices;
				faces = right.faces;
			}
			return *this;
		}
		const std::size_t ByteWidth() const { return sizeof(this->vertices.front()) * this->vertices.size(); } //returns the size of the vertex array in bytes for 
	};

	//CONSTANT BUFFER OBJECT TYPES============================================================================================================
	struct World : public DirectX::XMFLOAT4X4 //for world-view-projection matrix
	{
		DirectX::XMFLOAT4X4 matrix;
	};

	struct Camera : public DirectX::XMFLOAT4X3 //right-oriented camera //stores a 4x3 view matrix
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 lookat;
		DirectX::XMFLOAT4 up;
	};

	struct Projection // projection, stores data for WVP matrix
	{
		float fov;
		float asratio;
		float nearz;
		float farz;
	};

	_declspec(align(16))
		struct WVP
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX project;
	};


#define TESTING 1
	//This doesn't
#if 1
	_declspec(align(16)) 
		struct SingleLight
	{
		float4 pos;
		float4 color;
		float4 camerapos;
		float ambient;
		float3 filler;
	};
#endif
#if 0
	//This works
	_declspec(align(16))
		struct SingleLight
	{
		DirectX::XMVECTOR pos;
		DirectX::XMVECTOR color;
		DirectX::XMVECTOR camerapos;
		float ambient;
	};
#endif
	_declspec(align(16))
	struct SimpleMaterial
	{
		float3 spec_color;
		float  spec_factor;
	};

	struct ImageData
	{
		int width;
		int height;
		int channels; //max 4, rgba
		std::vector<unsigned char> data;
		ImageData(int _width = 0, int _height = 0, int _channels = 3) : width(_width), height(_height), channels(_channels) {};
		~ImageData(){};
	};
}