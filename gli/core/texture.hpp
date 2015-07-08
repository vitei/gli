///////////////////////////////////////////////////////////////////////////////////
/// OpenGL Image (gli.g-truc.net)
///
/// Copyright (c) 2008 - 2015 G-Truc Creation (www.g-truc.net)
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
///
/// @ref core
/// @file gli/core/texture.hpp
/// @date 2013-02-05 / 2013-02-05
/// @author Christophe Riccio
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "image.hpp"

namespace gli
{
	class texture
	{
	public:
		typedef unsigned char data_type;
		typedef size_t size_type;
		typedef gli::format format_type;
		typedef dim1_t dim1_type;
		typedef dim2_t dim2_type;
		typedef dim3_t dim3_type;
		typedef dim4_t dim4_type;
		typedef texcoord1_t texcoord1_type;
		typedef texcoord2_t texcoord2_type;
		typedef texcoord3_t texcoord3_type;
		typedef texcoord4_t texcoord4_type;

		texture();

		texture(
			size_type const & Layers,
			size_type const & Faces,
			size_type const & Levels,
			format_type const & Format,
			dim3_type const & Dimensions);

		texture(
			texture const & Texture,
			format_type const & Format,
			size_type BaseLayer, size_type MaxLayer,
			size_type BaseFace, size_type MaxFace,
			size_type BaseLevel, size_type MaxLevel);

		bool empty() const;
		format_type format() const;

		size_type width() const;
		size_type height() const;
		size_type depth() const;

		size_type baseLayer() const;
		size_type maxLayer() const;
		size_type layers() const;

		size_type baseFace() const;
		size_type maxFace() const;
		size_type faces() const;

		size_type baseLevel() const;
		size_type maxLevel() const;
		size_type levels() const;

		dim3_type dimensions(size_type const & Level = 0) const;

		/// Texture size expressed in number of bytes.
		size_type size() const;

		/// Texture size expressed in number of texels where genType must match the texture format.
		template <typename genType>
		size_type size() const;

		void * data();
		template <typename genType>
		genType * data();
		void const * data() const;
		template <typename genType>
		genType const * data() const;

		void clear();
		template <typename genType>
		void clear(genType const & Texel);

	private:
		void * const compute_data();
		size_type compute_size() const;

		size_type level_size(
			size_type const & Level) const;
		size_type face_size(
			size_type const & BaseLevel, size_type const & MaxLevel) const;
		size_type layer_size(
			size_type const & BaseFace, size_type const & MaxFace,
			size_type const & BaseLevel, size_type const & MaxLevel) const;
		size_type imageAddressing(
			size_type const & LayerOffset,
			size_type const & FaceOffset,
			size_type const & LevelOffset);

	protected:
		std::shared_ptr<impl> Impl;
		format_type const Format;
		size_type const BaseLayer;
		size_type const MaxLayer;
		size_type const BaseFace;
		size_type const MaxFace;
		size_type const BaseLevel;
		size_type const MaxLevel;
		void * const Data;
		size_type const Size;
	};

	inline texture::texture()
		: Format(static_cast<gli::format>(FORMAT_INVALID))
		, BaseLayer(0), MaxLayer(0)
		, BaseFace(0), MaxFace(0)
		, BaseLevel(0), MaxLevel(0)
		, Data(nullptr)
		, Size(0)
	{}

	inline texture::texture
	(
		size_type const & Layers,
		size_type const & Faces,
		size_type const & Levels,
		format_type const & Format,
		dim3_type const & Dimensions
	)
		: Impl(new impl(Layers, Faces, Levels, Format, Dimensions))
		, Format(Format)
		, BaseLayer(0), MaxLayer(Layers - 1)
		, BaseFace(0), MaxFace(Faces - 1)
		, BaseLevel(0), MaxLevel(Levels - 1)
		, Data(this->compute_data())
		, Size(this->compute_size())
	{
		assert(Layers > 0);
		assert(Faces > 0);
		assert(Levels > 0);
		assert(glm::all(glm::greaterThan(Dimensions, dim3_type(0))));
	
		this->Impl->Data.resize(this->layer_size(0, Faces - 1, 0, Levels - 1) * Layers, 0);
	}

	inline texture::texture
	(
		texture const & Texture,
		format_type const & Format,
		size_type BaseLayer, size_type MaxLayer,
		size_type BaseFace, size_type MaxFace,
		size_type BaseLevel, size_type MaxLevel
	)
		: Impl(Texture.Impl)
		, Format(Format)
		, BaseLayer(BaseLayer), MaxLayer(MaxLayer)
		, BaseFace(BaseFace), MaxFace(MaxFace)
		, BaseLevel(BaseLevel), MaxLevel(MaxLevel)
		, Data(Texture.Data)
		, Size(Texture.Size)
	{}

	inline texture::size_type texture::size() const
	{
		assert(!this->empty());

		return this->Size;
	}

	template <typename genType>
	inline texture::size_type texture::size() const
	{
		assert(!this->empty());
		assert(block_size(this->format()) >= sizeof(genType));

		return this->size() / sizeof(genType);
	}

	inline void * texture::data()
	{
		return this->Data;
	}

	inline void const * texture::data() const
	{
		return this->Data;
	}

	template <typename genType>
	inline genType * texture::data()
	{
		assert(!this->empty());
		assert(block_size(this->format()) >= sizeof(genType));

		return reinterpret_cast<genType *>(this->data());
	}

	template <typename genType>
	inline genType const * texture::data() const
	{
		assert(!this->empty());
		assert(block_size(this->format()) >= sizeof(genType));

		return reinterpret_cast<genType const *>(this->data());
	}

	inline bool texture::empty() const
	{
		return this->data() == nullptr;
	}

	inline texture::format_type texture::format() const
	{
		return this->Format;
	}

	inline texture::size_type texture::width() const
	{
		return this->Impl->Dimensions.x;
	}

	inline texture::size_type texture::height() const
	{
		return this->Impl->Dimensions.y;
	}

	inline texture::size_type texture::depth() const
	{
		return this->Impl->Dimensions.z;
	}

	inline texture::size_type texture::baseLayer() const
	{
		return this->BaseLayer;
	}

	inline texture::size_type texture::maxLayer() const
	{
		return this->MaxLayer;
	}

	inline texture::size_type texture::layers() const
	{
		return this->maxLayer() - this->baseLayer() + 1;
	}

	inline texture::size_type texture::baseFace() const
	{
		return this->BaseFace;
	}

	inline texture::size_type texture::maxFace() const
	{
		return this->MaxFace;
	}

	inline texture::size_type texture::faces() const
	{
		//assert(this->maxFace() - this->baseFace() + 1 == 1);
		return this->maxFace() - this->baseFace() + 1;
	}

	inline texture::size_type texture::baseLevel() const
	{
		return this->BaseLevel;
	}

	inline texture::size_type texture::maxLevel() const
	{
		return this->MaxLevel;
	}

	inline texture::size_type texture::levels() const
	{
		return this->maxLevel() - this->baseLevel() + 1;
	}

	inline texture::dim3_type texture::dimensions(size_type const & Level) const
	{
		assert(Level < this->Impl->Levels);

		return glm::max(this->Impl->Dimensions >> texture::dim3_type(static_cast<glm::uint>(Level)), texture::dim3_type(static_cast<glm::uint>(1)));
	}

	inline void texture::clear()
	{
		memset(this->data<data_type>(), 0, this->size<data_type>());
	}

	template <typename genType>
	inline void texture::clear(genType const & Texel)
	{
		assert(!this->empty());
		assert(block_size(this->format()) == sizeof(genType));

		genType* Data = this->data<genType>();
		size_type const TexelCount = this->size<genType>();

		for(size_type TexelIndex = 0; TexelIndex < TexelCount; ++TexelIndex)
			*(Data + TexelIndex) = Texel;
	}

	void * const texture::compute_data()
	{
		size_type const offset = this->imageAddressing(
			this->baseLayer(), this->baseFace(), this->baseLevel());

		return &this->Impl->Data[0] + offset;
	}

	inline texture::size_type texture::compute_size() const
	{
		assert(!this->empty());

		return this->layer_size(
			this->baseFace(), this->maxFace(),
			this->baseLevel(), this->maxLevel()) * this->layers();
	}

	inline texture::size_type texture::level_size(size_type const & Level) const
	{
		assert(Level < this->levels());

		dim3_type const BlockDimensions(
			gli::block_dimensions_x(this->format()),
			gli::block_dimensions_y(this->format()),
			gli::block_dimensions_z(this->format()));
		dim3_type const Dimensions = this->dimensions(Level);
		dim3_type const Multiple = glm::ceilMultiple(Dimensions, BlockDimensions);
		std::size_t const BlockSize = gli::block_size(this->format());

		return BlockSize * glm::compMul(Multiple / BlockDimensions);
	}

	inline texture::size_type texture::face_size(
		size_type const & BaseLevel, size_type const & MaxLevel) const
	{
		assert(MaxLevel < this->levels());
		
		size_type FaceSize(0);

		// The size of a face is the sum of the size of each level.
		for(size_type Level(BaseLevel); Level <= MaxLevel; ++Level)
			FaceSize += this->level_size(Level);

		return FaceSize;
	}

	inline texture::size_type texture::layer_size(
		size_type const & BaseFace, size_type const & MaxFace,
		size_type const & BaseLevel, size_type const & MaxLevel) const
	{
		assert(MaxFace < this->faces());
		assert(MaxLevel < this->levels());

		// The size of a layer is the sum of the size of each face.
		// All the faces have the same size.
		return this->face_size(BaseLevel, MaxLevel) * (MaxFace - BaseFace + 1);
	}

	inline texture::size_type texture::imageAddressing
	(
		texture::size_type const & LayerOffset,
		texture::size_type const & FaceOffset,
		texture::size_type const & LevelOffset
	)
	{
		assert(LayerOffset < this->layers());
		assert(FaceOffset < this->faces());
		assert(LevelOffset < this->levels());

		size_type LayerSize = this->layer_size(0, this->faces() - 1, 0, this->levels() - 1);
		size_type FaceSize = this->face_size(0, this->levels() - 1);
		size_type BaseOffset = LayerSize * LayerOffset + FaceSize * FaceOffset; 

		for(size_type Level(0); Level < LevelOffset; ++Level)
			BaseOffset += this->level_size(Level);

		return BaseOffset;
	}
}//namespace gli

#include "texture1d.hpp"
#include "texture1d_array.hpp"
#include "texture2d.hpp"
#include "texture2d_array.hpp"
#include "texture3d.hpp"
#include "texture_cube.hpp"
#include "texture_cube_array.hpp"

#include "addressing.hpp"

#include "texture1d.inl"
#include "texture1d_array.inl"
#include "texture2d.inl"
#include "texture2d_array.inl"
#include "texture3d.inl"
#include "texture_cube.inl"
#include "texture_cube_array.inl"
