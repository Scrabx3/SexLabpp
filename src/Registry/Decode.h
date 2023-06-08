#pragma once

#include "Animation.h"

namespace Registry
{
	class Decoder
	{
	public:
		/// @brief Decode a .slr file into an AnimPackage
		/// @param a_file The file to decode
		/// @return A new package representing the files contents
		/// @throw if the file is empty or has errors
		_NODISCARD static std::unique_ptr<AnimPackage> Decode(const fs::path a_file);

	private:
		_NODISCARD static std::unique_ptr<AnimPackage> Version1(std::ifstream& a_stream);
	};

}	 // namespace Registry
