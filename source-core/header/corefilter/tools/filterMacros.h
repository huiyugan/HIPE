//READ LICENSE BEFORE ANY USAGE
/* Copyright (C) 2018  Damien DUBUC ddubuc@aneo.fr (ANEO S.A.S)
 *  Team Contact : hipe@aneo.fr
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *  
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 *  In addition, we kindly ask you to acknowledge ANEO and its authors in any program 
 *  or publication in which you use HIPE. You are not required to do so; it is up to your 
 *  common sense to decide whether you want to comply with this request or not.
 *  
 *  Non-free versions of HIPE are available under terms different from those of the General 
 *  Public License. e.g. they do not require you to accompany any object code using HIPE 
 *  with the corresponding source code. Following the new licensing any change request from 
 *  contributors to ANEO must accept terms of re-license by a general announcement. 
 *  For these alternative terms you must request a license from ANEO S.A.S Company 
 *  Licensing Office. Users and or developers interested in such a license should 
 *  contact us (hipe@aneo.fr) for more information.
 */

#pragma once
#include <core/Invoker.h>

#pragma warning(push, 0)
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#pragma warning(pop)


#define TO_STR(A) #A

#define CONNECTOR(DataIn, DataOut)\
	data::ConnexData<DataIn,DataOut> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define CONNECTOR_OUT(DataOut)\
	data::ConnexOutput<DataOut> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define CONNECTOR_IN(DataIn)\
	data::ConnexInput<DataIn> _connexData;\
	inline virtual data::ConnexDataBase & getConnector() { return _connexData; }

#define PUSH_DATA(DataIn)\
	DataIn.setLabel(this->getName() + "__dataOut");\
	_connexData.push(DataIn)


#define SET_NAMESPACE(_namespace)\
	virtual std::string getNamespace() const\
	{\
		return _namespace;\
	}


#define REGISTER(Constructor, params)\
	public:\
	\
	typedef ProxyFunctor<Constructor> _proxyFunctor; \
	typedef Constructor mytype;\
	Constructor(filter::_protectPreInit unused) : filter::IFilter(#Constructor)\
	{}\
	Constructor params : filter::IFilter(#Constructor)


#define EXPAND_VAR(elem) elem
#define CONCAT(a,b) a ## b
#define CONCAT2(a, b) CONCAT( a , b )
#define CONCAT3(a, b, c) CONCAT2(CONCAT2(a,b), c)
#define CONCAT4(a, b, c, d) CONCAT2(CONCAT3(a,b, c), d)
#define CONCAT5(a, b, c, d, e) CONCAT2(CONCAT4(a,b, c, d), e)
#define CONCAT2_STR(a, b) TO_STR(a ## b)
#define CONCAT3_STR(a, b, c) TO_STR(a ## b ## c)

#define STRINGIZE(x) x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)

#define REGISTER_P(typef, params)\
		typef params; \
		typedef typef vartype__##params; \
		public: \
		void set_##params##_from_json(::json::JsonTree & jsonClass) \
		{\
				this-> params = jsonClass.get<typef>(std::string (#params));\
		}\
		void set_##params(vartype__##params & __##params) \
		{\
				this-> params = __##params;\
		}\
		vartype__##params get_##params() { return this-> params; }\
		void get_json_##params(::json::JsonTree & jsonClass)\
		{\
			jsonClass.put<typef>(std::string (#params), this->params);\
		}\
		void copy_##params(mytype * instance) \
		{\
				this-> params = instance->params;\
		}

#define ADD_ARGS(r, classname, elem)\
	core::InvokerBase CONCAT4(invoke_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<void, CONCAT2(classname::vartype__, elem)&>::for_function<classname,  & CONCAT2(classname::set_, elem) >();\
	CONCAT4(std::string str_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(set_ , elem)), CONCAT4(invoke_set_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT5(invoke_set_, classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME) ) = core::Invoker<void, json::JsonTree &>::for_function<classname, & CONCAT3(classname::set_, elem, _from_json)>();\
	CONCAT5(std::string str_ , classname ## _ , elem, _from_json, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT3_STR(set_ , elem , _from_json)), CONCAT5(invoke_set_ , classname, elem, _from_json, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT4(invoke_get_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<CONCAT2(classname::vartype__, elem)>::for_function<classname, & CONCAT2(classname::get_, elem)>();\
	CONCAT4(std::string str_get_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(get_ , elem)), CONCAT4(invoke_get_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT4(invoke_copy_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<void, classname *>::for_function<classname, & CONCAT2(classname::copy_, elem)>();\
	CONCAT4(std::string str_copy_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(copy_ , elem)), CONCAT4(invoke_copy_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\
	core::InvokerBase CONCAT4(invoke_get_json_ , classname, elem, EXPAND_VAR(FILE_BASENAME))  = core::Invoker<void, json::JsonTree &>::for_function<classname, & CONCAT2(classname::get_json_, elem)>();\
	CONCAT4(std::string str_get_json_ , classname ## _ , elem, EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addSetter(std::string(TO_STR(classname)), std::string(CONCAT2_STR(get_json_ , elem)), CONCAT4(invoke_get_json_ , classname, elem, EXPAND_VAR(FILE_BASENAME)));\


#define ADD_CLASS(classname, ...)\
	std::string CONCAT3(str_constructor_, classname , EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addClass(std::string(#classname), [](){ return static_cast<classname *>(new classname()); }); \
	std::string CONCAT3(str_constructor_preload_, classname , EXPAND_VAR(FILE_BASENAME)) = RegisterTable::getInstance().addPreloadClass(std::string(#classname), [](){ return static_cast<classname *>(new classname(filter::_protectPreInit())); }); \
	BOOST_PP_SEQ_FOR_EACH(ADD_ARGS, classname, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define __callfunc(instance, function, ...) 	RegisterTable::getInstance().invoke(instance, function, __VA_ARGS__)

