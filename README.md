# C-Promises-Implementation

This implementation shows the basic idea of how Promises and their callbacks could be handled in the C++ Programming Language. In other words, it's the basic design of Promises in C++.

## Examples
Some of the examples taken from [<url> understanding-javascript-promises </url>](https://nodejs.dev/learn/understanding-javascript-promises)
	
	void e1(){
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

	void e2(){ // handling errors
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

	void e3(){ // cascading errors
		ECMA::Promise<int, char*>([](auto resolve, auto reject){
				throw (char*)"ERR";
		})._catch([](auto err){
				// std::cout << err << std::endl;
				throw (char*)"SEC";
		})._catch([](auto err){
			std::cout << err << std::endl;
		});
	}
