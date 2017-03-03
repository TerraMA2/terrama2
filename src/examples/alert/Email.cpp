#include <iostream>
#include <jwsmtp.h>
#include <cstring>

using std::cout;
using std::cin;
using std::string;

void Usage( ) {
  cout << "jwSMTP library demo program\n"
      "maildemo \n"
      " e.g.\n"
      " maildemo you@there.com me@here.com mail.here.com\n";
}

int main(int argc, char* argv[ ]) {
  if(argc != 4) {
    Usage( );
    return 0;
  }

  cout << "jwSMTP library demo program\n\n";
  string to = "paulo002@gmail.com";
  string from = "paulo002@gmail.com";
  string smtpserver = "smtp.gmail.com";

  std::string subject = "Test";

  string mailmessage = "This is a test message";

  // This is how to tell the mailer class that we are using a direct smtp server
  // preventing the code from doing an MX lookup on 'smtpserver'
  jwsmtp::mailer mail(to.c_str( ), from.c_str( ), subject.c_str( ), mailmessage.c_str( ),
                      smtpserver.c_str( ), jwsmtp::mailer::SMTP_PORT, false);

  // using a local file as opposed to a full path.
  mail.attach("/home/paulo/Downloads/projeto_bugado.terrama2");

  // boost::thread thrd(mail);
  // thrd.join( ); // optional
  // or:-
  mail.operator ( )( );
  cout << mail.response( ) << "\n";

  return 0;
}