//
// Created by ufo on 18-3-14.
//

#include "core/app-template.hh"
#include "core/reactor.hh"
#include <iostream>
#include <stdexcept>
int main(int argc, char** argv) {
    seastar::app_template app;
    try {
        app.run(argc, argv, [] {
            std::cout << "Thread count: " << seastar::smp::count << "\n";
            std::cout << "Hello world!" << std::endl;
            return seastar::make_ready_future<>();
        });
    } catch(...) {
        std::cerr << "Failed to start application: "
                  << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
