#pragma once

template <typename T>
class Combinator
{
	typedef typename std::vector<T>::iterator Iterator;

public:
	Combinator(std::vector<std::vector<T>>& a_data) :
		data(a_data)
	{
		current.reserve(data.size());
		for (auto& v : data)
			current.push_back(v.begin());
	}

	bool next()
	{
		for (size_t i = data.size() - 1; i >= 0; i--) {
			auto& v = data[i];
			auto& it = current[i];

			if (++it != v.end())
				return true;
			it = v.begin();
		}
		return false;
	}

	std::vector<Iterator> combination() const
	{
		return current;
	}

private:
	std::vector<std::vector<T>>& data;
	std::vector<Iterator> current;
};
