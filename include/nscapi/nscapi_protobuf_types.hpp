/*
 * Copyright 2004-2016 The NSClient++ Authors - https://nsclient.org
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <list>
#include <set>
#include <map>

#include <NSCAPI.h>
#include <net/net.hpp>
#include <strEx.h>
#include <nscapi/dll_defines.hpp>

namespace nscapi {
	namespace protobuf {
		namespace types {
			struct NSCAPI_EXPORT decoded_simple_command_data {
				std::string command;
				std::string target;
				std::list<std::string> args;
			};
		}
	}
}