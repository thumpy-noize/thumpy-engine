

#include <input_manager.hpp>
#include <stdlib.h>
#include <iostream>
#include <curses.h>
#include <signal.h>

namespace Thumpy {
  namespace Core {
    namespace IO {

      static void finish(int sig);

      void init() {

        ( void ) signal( SIGINT, finish );

        ( void ) initscr();
        keypad( stdscr, TRUE );
        ( void  ) nonl();
        ( void  ) cbreak();
        ( void  ) echo();
      }


      static void finish( int sig  )
      {
        endwin();

        // Shutdown
        
        std::cout << "Shuting down..." << std::endl;

        exit(0);

      }

      void poll_input() {

        // Poll terminal input (dev)
        poll_terminal_values();

      }

      void poll_terminal_values()
      {
        //std::cout << "Polling term..." << std::endl;
        
        int buf = getch();

        std::cout << "char: " << (int)buf << std::endl;


      }
    }
  }
}

