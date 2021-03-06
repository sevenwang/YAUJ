#include<typeinfo>
#include<exception>
#include<stdexcept>
#include<sstream>
#include "interpreter.h"

// CLASS v_base
v_base::~v_base() {}
int v_base::to() const
{
	return 0;
}
int v_base::as_int() const
{
	throw std::runtime_error("error converting to int");
}
double v_base::as_float() const
{
	throw std::runtime_error("error converting to float");
}
bool v_base::as_bool() const
{
	throw std::runtime_error("error converting to bool");
}
Json::Value v_base::as_json() const
{
	return Json::nullValue;
}
std::string v_base::as_str() const
{
	throw std::runtime_error("error converting to str");
}
std::vector<iter> &v_base::as_list()
{
	throw std::runtime_error("error converting to list");
}
std::map<std::string,iter> &v_base::as_dict()
{
	throw std::runtime_error("error converting to dict");
}
v_base_ptr v_base::clone() const
{
	throw std::logic_error("attempt to access base class");
}

// CLASS v_int
int v_int::as_int() const
{
	return data;
}
double v_int::as_float() const
{
	return data;
}
bool v_int::as_bool() const
{
	return data;
}
Json::Value v_int::as_json() const
{
	return data;
}
std::string v_int::as_str() const
{
	std::ostringstream ss;
	ss << data;
	return ss.str();
}
v_base_ptr v_int::clone() const
{
	return v_base_ptr(new v_int(data));
}

// CLASS v_float
double v_float::as_float() const
{
	return data;
}
bool v_float::as_bool() const
{
	return data;
}
Json::Value v_float::as_json() const
{
	return data;
}
std::string v_float::as_str() const
{
	std::ostringstream ss;
	ss << data;
	return ss.str();
}
v_base_ptr v_float::clone() const
{
	return v_base_ptr(new v_float(data));
}

// CLASS v_bool
int v_bool::as_int() const
{
	return data;
}
double v_bool::as_float() const
{
	return data;
}
bool v_bool::as_bool() const
{
	return data;
}
Json::Value v_bool::as_json() const
{
	return data;
}
v_base_ptr v_bool::clone() const
{
	return v_base_ptr(new v_bool(data));
}

// CLASS v_str
bool v_str::as_bool() const
{
	return ! data.empty();
}
Json::Value v_str::as_json() const
{
	return data;
}
std::string v_str::as_str() const
{
	return data;
}
v_base_ptr v_str::clone() const
{
	return v_base_ptr(new v_str(data));
}

// CLASS v_list
bool v_list::as_bool() const
{
	return ! data.empty();
}
Json::Value v_list::as_json() const
{
	Json::Value ret;
	for (size_t i=0;i<data.size();i++) if (static_cast<bool>(data[i].ptr))
		ret[Json::ArrayIndex(i)]=data[i]->as_json();
	return ret;
}
std::vector<iter> &v_list::as_list()
{
	return data;
}
v_base_ptr v_list::clone() const
{
	return v_base_ptr(new v_list(data));
}

// CLASS v_dict
bool v_dict::as_bool() const
{
	return ! data.empty();
}
Json::Value v_dict::as_json() const
{
	Json::Value ret;
	for (const auto &x : data) if (static_cast<bool>(x.second.ptr))
		ret[x.first]=x.second->as_json();
	return ret;
}
std::map<std::string,iter> &v_dict::as_dict()
{
	return data;
}
v_base_ptr v_dict::clone() const
{
	return v_base_ptr(new v_dict(data));
}

// CLASS iter

iter &iter::operator=(const iter &x)
{
	if (! static_cast<bool> (x.ptr))
		ptr=0;
	else 
		ptr=x.ptr->clone();
	return *this;
}

v_base &iter::operator*() const
{
	if (ptr) return *ptr;
	throw std::runtime_error("Attempt to access NONE");
}

v_base_ptr iter::operator->() const
{
	if (ptr) return ptr;
	throw std::runtime_error("Attempt to access NONE");
}

iter::operator bool() const
{
	return static_cast<bool>(ptr) && ptr->as_bool();
}

iter iter::operator!() const
{
	return _I_(new v_bool(! (bool) *this));
}

iter iter::operator-() const
{
	return _I_(new v_int(0))-*this;
}

iter &iter::operator[](const iter &x)
{
	if (! static_cast<bool>(ptr) && (x->to() & INT))
		(*this) = _I_(new v_list());
	if (static_cast<bool>(ptr) && ((*this)->to() & LIST) && (x->to() & INT))
	{
		int id(x->as_int());
		std::vector<iter> &V=(*this)->as_list();
		if (id<0) id+=V.size();
		if (id<0)
			throw std::runtime_error("the subscript is too low");
		while ((size_t)id>=V.size()) V.push_back(_I_(0));
		return (*this)->as_list()[id];
	}
	if (! static_cast<bool>(ptr) && (x->to() & STR))
		(*this) = _I_(new v_dict());
	if (((*this)->to() & DICT) && (x->to() & STR))
		return (*this)->as_dict()[x->as_str()];
	throw std::runtime_error("no matched operator []");
}

iter &iter::operator++()
{
	if (((*this)->to() & INT) || ((*this)->to() & FLOAT))
		return (*this) = (*this) + _I_(new v_int(1));
	throw std::runtime_error("no matched operator ++");
}

iter &iter::operator--()
{
	if (((*this)->to() & INT) || ((*this)->to() & FLOAT))
		return (*this) = (*this) - _I_(new v_int(1));
	throw std::runtime_error("no matched operator --");
}

iter iter::operator++(int)
{
	if (((*this)->to() & INT) || ((*this)->to() & FLOAT))
	{
		iter ret(*this);
		(*this) = (*this) + _I_(new v_int(1));
		return ret;
	}
	throw std::runtime_error("no matched operator ++");
}

iter iter::operator--(int)
{
	if (((*this)->to() & INT) || ((*this)->to() & FLOAT))
	{
		iter ret(*this);
		(*this) = (*this) - _I_(new v_int(1));
		return ret;
	}
	throw std::runtime_error("no matched operator --");
}

iter &iter::add(const iter &x)
{
	if (! static_cast<bool> (ptr))
		(*this) = _I_(new v_list());
	if ((*this)->to() & LIST)
	{
		(*this)->as_list().push_back(x);
		return *this;
	}
	throw std::runtime_error("no matched method add");
}

iter &iter::add(const std::pair<std::string,iter> &x)
{
	if ((*this)->to() & DICT)
	{
		(*this)->as_dict().insert(x);
		return *this;
	}
	throw std::runtime_error("no matched method add");
}


// OUTSIDE OPERATORS
iter operator+(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_int(a->as_int() + b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_float(a->as_float() + b->as_float()));
	if ((a->to() & STR) && (b->to() & STR))
		return _I_(new v_str(a->as_str() + b->as_str()));
	if ((a->to() & LIST) && (b->to() & LIST))
	{
		iter ret = a;
		for (const iter x : b->as_list()) a->as_list().push_back(x);
		return ret;
	}
	throw std::runtime_error("no matched operator +");
}

iter operator-(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_int(a->as_int() - b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_float(a->as_float() - b->as_float()));
	throw std::runtime_error("no matched operator -");
}

iter &operator+=(iter &a, const iter &b)
{
	return a = a + b;
}

iter &operator-=(iter &a, const iter &b)
{
	return a = a - b;
}

iter operator*(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_int(a->as_int() * b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_float(a->as_float() * b->as_float()));
	if ((a->to() & LIST) && (b->to() & INT) && b->as_int()>=0)
	{
		iter ret = _I_(new v_list());
		for (int i=1;i<=b->as_int();i++) ret += a;
		return ret;
	}
	if ((b->to() & LIST) && (a->to() & INT) && a->as_int()>=0)
	{
		iter ret = _I_(new v_list());
		for (int i=1;i<=a->as_int();i++) ret += b;
		return ret;
	}
	throw std::runtime_error("no matched operator *");
}

iter operator/(const iter &a, const iter &b)
{
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_float(a->as_float() / b->as_float()));
	throw std::runtime_error("no matched operator /");
}

iter operator%(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_int(a->as_int() % b->as_int()));
	throw std::runtime_error("no matched operator MOD");
}

iter operator<(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_bool(a->as_int() < b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_bool(a->as_float() < b->as_float()));
	if ((a->to() & STR) && (b->to() & STR))
		return _I_(new v_bool(a->as_str() < b->as_str()));
	if ((a->to() & LIST) && (b->to() & LIST))
		return _I_(new v_bool(a->as_list() < b->as_list()));
	throw std::runtime_error("no matched comparison operator");
}

iter operator>(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_bool(a->as_int() > b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_bool(a->as_float() > b->as_float()));
	if ((a->to() & STR) && (b->to() & STR))
		return _I_(new v_bool(a->as_str() > b->as_str()));
	if ((a->to() & LIST) && (b->to() & LIST))
		return _I_(new v_bool(a->as_list() > b->as_list()));
	throw std::runtime_error("no matched comparison operator");
}

iter operator<=(const iter &a, const iter &b)
{
	return ! (a>b);
}

iter operator>=(const iter &a, const iter &b)
{
	return ! (a<b);
}

iter operator==(const iter &a, const iter &b)
{
	if ((a->to() & INT) && (b->to() & INT))
		return _I_(new v_bool(a->as_int() == b->as_int()));
	if ((a->to() & FLOAT) && (b->to() & FLOAT))
		return _I_(new v_bool(a->as_float() == b->as_float()));
	if ((a->to() & STR) && (b->to() & STR))
		return _I_(new v_bool(a->as_str() == b->as_str()));
	if ((a->to() & LIST) && (b->to() & LIST))
		return _I_(new v_bool(a->as_list() == b->as_list()));
	throw std::runtime_error("no matched equality operator");
}

iter operator!=(const iter &a, const iter &b)
{
	return ! (a==b);
}

/*iter operator&&(const iter &a, const iter &b)
{
	return _I_(new v_bool((bool)a && (bool)b));
}

iter operator||(const iter &a, const iter &b)
{
	return _I_(new v_bool((bool)a || (bool)b));
}*/

// OUTSIDE FUNCTIONS
iter FEQ(const iter &a, const iter &b)
{
	return _I_(new v_bool(typeid(*a) == typeid(*b) && a == b));
}

iter NFEQ(const iter &a, const iter &b)
{
	return ! FEQ(a,b);
}

void foreach(const iter &a, void(*work)(const iter&))
{
	if (a->to() & LIST)
		for (const auto &x : a->as_list())
			work(x);
	else
	if (a->to() & DICT)
		for (const auto &x : a->as_dict())
			work(x.second);
	else
	throw std::runtime_error("neither a list nor a dict");
}

void foreach(const iter &a, void(*work)(const iter&, const iter&))
{
	if (a->to() & LIST)
	{
		const std::vector<iter> &V=a->as_list();
		for (int i=0;(size_t)i<V.size();i++)
			work(_I_(new v_int(i)),V[i]);
	} else
	if (a->to() & DICT)
		for (const auto &x : a->as_dict())
			work(_I_(new v_str(x.first)),x.second);
	else
	throw std::runtime_error("neither a list nor a dict");
}

