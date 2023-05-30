#pragma once

#include <shared_mutex>
#include "Animation.h"

namespace Registry
{
  class Library : public Singleton<Library>
	{
	public:
		void Initialize();

	private:
		mutable std::shared_mutex read_write_lock;

    std::vector<AnimPackage> packages;
	};
}