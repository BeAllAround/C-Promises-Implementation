#include <functional>

// template<class call, class...types>using _function = call(*)(types...); // tried to implement a namespace akin to std::function, but decided to stick to std::function in the long run


template<typename T,class TR=T> // the same embedded implementation of ECMA Promises in C++
class scope{
	// template<class...types>using _TYPEC = _function<void, _function<void, T>, _function<void, TR>, types...>;

	protected:private:
		T resolveValue;
		TR rejectValue;


	       	bool c = false;
		bool thenCheck = true, catchCheck = true;
		// string state = "pending...";

	        T resolveBack(T arg){
			return arg;
		};

		TR rejectBack(TR arg){
			return arg;

		};

		scope(void); // cloak the constructor;

		void _resolve(T args){
			if(!c){
				resolveValue = resolveBack(args);
				c = true;
			}
		};
	public:
		void _reject(TR args){
			if(!c){
				rejectValue = rejectBack(args);
				c = true;

			}
		};
		scope(std::function<void(std::function<void(T)>, std::function<void(TR)>)> callback){
				auto res = [this](T ar)->void{
					this->_resolve(ar);
				};
				auto rej = [this](TR ar)->void{
					this->_reject(ar);
				};
				try{
					callback(res, rej);
				}catch(TR err){
					_reject(err);
				}
		}

		scope(std::function<void(std::function<void(T)>)> callback){
			auto res = [this](T ar)->void{ // bind design
				this->_resolve(ar);
			};
			try{
				callback(res);
			}catch(TR err){
				_reject(err);
			}
		}

		static scope<T, TR> resolve(T);
		static scope<T, TR> reject(TR);

		scope<T, TR> then(std::function<void(T, TR)>);

		scope<T, TR> then(std::function<void(T)>);

		scope<T, TR>_catch(std::function<void(TR)>);

		scope<T, TR> then(void);

		T& await(void);

		template<typename...types>
			static scope<T, TR> async(std::function<T(types...)>, types...);
};

// specifications
template<class T, class TR>
scope<T, TR> scope<T, TR>::resolve(T item){
	return scope<T, TR>([item](auto resolve){
			resolve(item);
	});
};

template<class T, class TR>
scope<T, TR> scope<T, TR>::reject(TR item){
	return scope<T, TR>([item](std::function<void(T)> resolve, std::function<void(TR)> reject){
			reject(item);
	});
};

template<class T, class TR>
scope<T, TR> scope<T, TR>::then(std::function<void(T, TR)> callback){
	if(thenCheck)
		callback(resolveValue, rejectValue), thenCheck = false;
	return *this;
};

template<class T, class TR>
scope<T, TR> scope<T, TR>::then(std::function<void(T)> callback){
	if(thenCheck)
		callback(resolveValue), thenCheck = false;
	return *this;
}

template<class T, class TR>
scope<T, TR> scope<T, TR>::then(){
	return *this;
};

template<class T, class TR>
scope<T, TR> scope<T, TR>::_catch(std::function<void(TR)> callback){
	try{
		if(catchCheck)
			callback(rejectValue), catchCheck = false;
	}catch(TR err){
		return scope::reject(err);
	}
	return *this;

}

template<class T, class TR>
T& scope<T, TR>::await(){
	if(!thenCheck)
		throw std::exception();
	thenCheck = false;
	return resolveValue;
};

template<class T, class TR>
scope<T, TR>::scope(void){
};

template<class T, class TR>
template<class...types>
scope<T, TR> scope<T, TR>::async(std::function<T(types...)> callback, types...args){
	return resolve(callback(args...));
};


namespace ECMA{
	template<class T, class TR=T>
		using Promise = scope<T, TR>;

	// custom lambda wrappers
	
	// template<class TYPE_, class TYPE__, typename...types>
		// using wrap = _function<void, _function<void, TYPE_>, _function<void, TYPE__>, types...>;

	// template<class TYPE_, typename...types>
		// using wrapAsync = _function<TYPE_, types...>;
};
