#pragma once

#include <boost/program_options.hpp>
#include "settings_client.hpp"

namespace po = boost::program_options;
class cli_parser {
	

	NSClient* core_;
	po::options_description desc;
	po::options_description settings;
	po::options_description service;

public:
	cli_parser(NSClient* core) 
		: core_(core)
		, desc("Allowed options")
		, settings("Settings options")
		, service("Service Options")
	{
		desc.add_options()
			("help", "produce help message")
			("settings-help", "produce help message for the various settings related options")
			("service-help", "produce help message for the various settings related options")
			("settings", "Enter settings mode and handle settings related commands")
			("service", "Enter service mode and handle service related commands")
			("test", "Start test and debug mode")
			("debug", "Show debug information")
			;


		settings.add_options()
			("migrate-to", po::value<std::wstring>(), "Migrate (copy) settings from current store to target store")
			("migrate-from", po::value<std::wstring>(), "Migrate (copy) settings from current store to target store")
			("generate", po::value<std::wstring>(), "(re)Generate a commented settings store or similar KEY can be trac, settings or the target store.")
			("add-defaults", "Add all default (if missing) values.")
			("path", po::value<std::wstring>()->default_value(_T("")), "Path of key to work with.")
			("key", po::value<std::wstring>()->default_value(_T("")), "Key to work with.")
			("set", po::value<std::wstring>(), "Set a key and path to a given value.")
			("show", "Set a value given a key and path.")
			("list", "Set all keys below the path (or root).")
			;

		service.add_options()
			("install", po::value<std::wstring>(), "Install service")
			;

	}
	int parse(int argc, wchar_t* argv[]) {
		try {
			po::options_description all("Allowed options");
			all.add(desc).add(service).add(settings);

			po::variables_map vm;
			po::wparsed_options parsed = 
				po::wcommand_line_parser(argc, argv).options(all).allow_unregistered().run();

			//po::store(po::parse_command_line(argc, argv, desc), vm);
			po::store(parsed, vm);
			po::notify(vm);

			if (vm.count("debug")) {
				std::wcout << _T("Enabling debug mode")<<std::endl;
				core_->enableDebug(true);
			}


			if (vm.count("settings")) {
				return parse_settings(argc, argv);
			}
			if (vm.count("test")) {
				return parse_test(argc, argv);
			}
			if (vm.count("help")) {
				std::cout << all << "\n";
				return 1;
			}
			if (vm.count("settings-help")) {
				std::cout << settings << "\n";
				return 1;
			}
			if (vm.count("service-help")) {
				std::cout << service << "\n";
				return 1;
			}
		} catch(std::exception & e) {
			std::cerr << "Unable to parse command line: " << e.what() << std::endl;
			return 1;
		} catch (...) {
			std::cerr << "Unhanded Exception" << std::endl;
			return 1;
		}
		return 0;
	}

	int parse_test(int argc, wchar_t* argv[]) {
		bool server = false;
// 		if (argc > 2 && wcscasecmp( _T("server"), argv[2] ) == 0 ) {
// 			server = true;
// 		}
// 		std::wcout << "Launching test mode - " << (server?_T("server mode"):_T("client mode")) << std::endl;
// 		LOG_MESSAGE_STD(_T("Booting: ") SZSERVICEDISPLAYNAME );
#ifdef WIN32
		try {
			if (serviceControll::isStarted(SZSERVICENAME)) {
				std::wcerr << "Service seems to be started, this is probably not a good idea..." << std::endl;
			}
		} catch (...) {
			// Empty by design
		}
#endif
		nsclient::simple_client client(core_);
		client.start();
		return 0;
	}

	int parse_settings(int argc, wchar_t* argv[]) {
		try {
			po::options_description all("Allowed options (settings)");
			all.add(desc).add(settings);

			po::variables_map vm;
			po::store(po::parse_command_line(argc, argv, all), vm);
			po::notify(vm);

			if (vm.count("help")) {
				std::cout << all << "\n";
				return 1;
			}
			bool def = vm.count("add-defaults")==1;

			nsclient::settings_client client(core_);

			std::wstring current = _T(""); //client.get_source();


			client.set_current(current);
			client.set_update_defaults(def);

			client.boot();

			int ret = -1;

			if (vm.count("migrate-to")) {
				ret = client.migrate_to(vm["migrate-to"].as<std::wstring>());
			}
			if (vm.count("migrate-from")) {
				ret = client.migrate_from(vm["migrate-from"].as<std::wstring>());
			}
			if (vm.count("generate")) {
				ret = client.generate(vm["generate"].as<std::wstring>());
			}
			if (vm.count("set")) {
				ret = client.set(vm["path"].as<std::wstring>(), vm["key"].as<std::wstring>(), vm["set"].as<std::wstring>());
			}
			if (vm.count("list")) {
				ret = client.list(vm["path"].as<std::wstring>());
			}
			if (vm.count("show")) {
				ret = client.show(vm["path"].as<std::wstring>(), vm["key"].as<std::wstring>());
			}
			client.exit();

			return ret;
		} catch(std::exception & e) {
			std::cerr << "Unable to parse command line (settings): " << e.what() << std::endl;
			return 1;
		}
	}

};




