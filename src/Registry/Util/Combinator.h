#pragma once

template <typename T>
class Combinator
{
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
		for (long long i = data.size() - 1; i >= 0; --i) {
			auto& v = data[i];
			auto& it = current[i];

			if (++it != v.end())
				return true;
			it = v.begin();
		}
		return false;
	}

	std::vector<std::vector<T>::iterator> combination() const
	{
		return current;
	}

private:
	std::vector<std::vector<T>>& data;
	std::vector<std::vector<T>::iterator> current;
};
