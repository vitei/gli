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
/// @file gli/core/image.hpp
/// @date 2011-10-06 / 2013-01-12
/// @author Christophe Riccio
///
/// @defgroup core_image Image 
/// @ingroup core
///////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "storage.hpp"
#include "addressing.hpp"
#include "header.hpp"

namespace gli
{
	struct impl
	{
		typedef unsigned char data_type;
		typedef dim3_t dim_type;
		typedef size_t size_type;
		typedef gli::format format_type;

		impl();

		explicit impl(
			size_type const & Layers,
			size_type const & Faces,
			size_type const & Levels,
			format_type const & Format,
			dim_type const & Dimensions);

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

		dim_type dimensions(size_type const & Level) const;

		size_type const Layers;
		size_type const Faces;
		size_type const Levels;
		format_type const Format;
		dim_type const Dimensions;
		std::vector<data_type> Data;
	};

	inline impl::impl() :
		Layers(0),
		Faces(0),
		Levels(0),
		Format(static_cast<gli::format>(FORMAT_INVALID)),
		Dimensions(0)
	{}

	inline impl::impl(
		size_type const & Layers,
		size_type const & Faces,
		size_type const & Levels,
		format_type const & Format,
		dim_type const & Dimensions
	)
		: Layers(Layers)
		, Faces(Faces)
		, Levels(Levels)
		, Format(Format)
		, Dimensions(Dimensions)
	{}

	inline impl::dim_type impl::dimensions(size_type const & Level) const
	{
		assert(Level < this->Levels);

		return glm::max(this->Dimensions >> dim_type(static_cast<glm::uint>(Level)), dim_type(static_cast<glm::uint>(1)));
	}

	inline impl::size_type impl::level_size(size_type const & Level) const
	{
		assert(Level < this->Levels);

		dim_type const BlockDimensions(
			gli::block_dimensions_x(this->Format),
			gli::block_dimensions_y(this->Format),
			gli::block_dimensions_z(this->Format));
		dim_type const Dimensions = this->dimensions(Level);
		dim_type const Multiple = glm::ceilMultiple(Dimensions, BlockDimensions);
		std::size_t const BlockSize = gli::block_size(this->Format);

		return BlockSize * glm::compMul(Multiple / BlockDimensions);
	}

	inline impl::size_type impl::face_size(
		size_type const & BaseLevel, size_type const & MaxLevel) const
	{
		assert(MaxLevel < this->Levels);
		
		size_type FaceSize(0);

		// The size of a face is the sum of the size of each level.
		for(size_type Level(BaseLevel); Level <= MaxLevel; ++Level)
			FaceSize += this->level_size(Level);

		return FaceSize;
	}

	inline impl::size_type impl::layer_size(
		size_type const & BaseFace, size_type const & MaxFace,
		size_type const & BaseLevel, size_type const & MaxLevel) const
	{
		assert(MaxFace < this->Faces);
		assert(MaxLevel < this->Levels);

		// The size of a layer is the sum of the size of each face.
		// All the faces have the same size.
		return this->face_size(BaseLevel, MaxLevel) * (MaxFace - BaseFace + 1);
	}

	inline impl::size_type impl::imageAddressing
	(
		impl::size_type const & LayerOffset,
		impl::size_type const & FaceOffset,
		impl::size_type const & LevelOffset
	)
	{
		assert(LayerOffset < this->Layers);
		assert(FaceOffset < this->Faces);
		assert(LevelOffset < this->Levels);

		size_type LayerSize = this->layer_size(0, this->Faces - 1, 0, this->Levels - 1);
		size_type FaceSize = this->face_size(0, this->Levels - 1);
		size_type BaseOffset = LayerSize * LayerOffset + FaceSize * FaceOffset; 

		for(size_type Level(0); Level < LevelOffset; ++Level)
			BaseOffset += this->level_size(Level);

		return BaseOffset;
	}

	/// Image
	class image
	{
	public:
		typedef dim1_t dim1_type;
		typedef dim2_t dim2_type;
		typedef dim3_t dim3_type;
		typedef dim3_type dim_type;
		typedef size_t size_type;
		typedef size_t layer_type;
		typedef size_t level_type;
		typedef size_t face_type;

		image();

		/// Allocate a new storage constructor
		explicit image(
			format const & Format,
			dim_type const & Dimensions);

		/// Reference an exiting storage constructor
		explicit image(
			std::shared_ptr<impl> Impl,
			size_type BaseLayer, size_type MaxLayer,
			size_type BaseFace, size_type MaxFace,
			size_type BaseLevel, size_type MaxLevel);

		bool empty() const;
		dim_type dimensions() const;

		size_type size() const;
		void * data();
		void const * data() const;

		template <typename genType>
		size_type size() const;
		template <typename genType>
		genType * data();
		template <typename genType>
		genType const * data() const;

		void clear();
		template <typename genType>
		void clear(genType const & Texel);
		template <typename genType>
		genType load(dim_type const & TexelCoord);
		template <typename genType>
		void store(dim_type const & TexelCoord, genType const & Data);

		size_type baseLayer() const;
		size_type maxLayer() const;
		size_type baseFace() const;
		size_type maxFace() const;
		size_type baseLevel() const;
		size_type maxLevel() const;

	private:
		std::shared_ptr<impl> Impl;
		size_type BaseLayer;
		size_type MaxLayer;
		size_type BaseFace;
		size_type MaxFace;
		size_type BaseLevel;
		size_type MaxLevel;
	};
}//namespace gli

#include "image.inl"
