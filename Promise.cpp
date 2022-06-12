#include <iostream>
#include <string>
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

class Error{ // class for exception-handling
        std::string message;
        public:
                Error()=default;
                Error(std::string message) : message{message}{
                }
                std::string show(){
                        return message;
                }
};

std::ostream&operator<<(std::ostream&out, Error err){
        out << err.show();
        return out;
}

// some of the examples provided taken from <url> understanding-javascript-promises </url>
void s1(){
	auto a1 = ECMA::Promise<int, int>([](auto resolve){
                        resolve(5101);
        });

        auto a2 = ECMA::Promise<int>([](auto resolve, auto reject){
                        reject(11);
        });

        a1.then([](auto resolve, auto reject){
                        std::cout << resolve << std::endl;
        });

        a2.then([](auto resolve, auto reject){
			std::cout << reject << std::endl;
       	});

	a1.then([](auto resolve){ // not able to "resolve" a Promise a second time
			std::cout << "S: " << resolve << std::endl;
	});

	// or

        ECMA::Promise<int, int>::resolve(5101).then([](auto resolve, auto reject){
                        std::cout << resolve << std::endl;
        });

	ECMA::Promise<int>::reject(11).then([](auto resolve, auto reject){
			std::cout << reject << std::endl;
	});

	// or even
	
	ECMA::Promise<int, int>::resolve(50).await(); // 50


	// how to use async 
	
	ECMA::Promise<int>::async((std::function<int(int)>)[](auto v){
			return v;
	}, 5).then([](auto resolve){
		std::cout << resolve << std::endl;
	});


}

void s2(){ // handling errors
	ECMA::Promise<std::string, Error>([](auto resolve, auto reject){
			throw Error("Error");
	})._catch([](auto err){
		std::cout << err << std::endl;
	});

	// or
	
	ECMA::Promise<std::string>([](auto resolve, auto reject){
			reject("Error");
	})._catch([](auto err){
		std::cout << err << std::endl;
	});
}

void s3(){ // cascading errors
	ECMA::Promise<int, char*>([](auto resolve, auto reject){
			throw (char*)"ERR";
	})._catch([](auto err){
			// std::cout << err << std::endl;
			throw (char*)"SEC";
	})._catch([](auto err){
		std::cout << err << std::endl;
	});

}

int main(void){
	s1();
	s2();
	s3();
	return 0;
};
