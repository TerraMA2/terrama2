#include <iostream>
#include <cstring>
#include <vmime/vmime.hpp>

#include <terrama2/services/alert/core/SimpleCertificateVerifier.hpp>

using std::cout;
using std::cin;
using std::string;

void Usage( ) {
    cout << "jwSMTP library demo program\n"
         "maildemo \n"
         " e.g.\n"
         " maildemo you@there.com me@here.com mail.here.com\n";
}

static vmime::shared_ptr <vmime::net::session> session = vmime::net::session::create();

void vmimeTest()
{
    vmime::messageBuilder mb;
    // Fill in the basic fields
    mb.setExpeditor(vmime::mailbox("vmimeteste@gmail.com"));
    vmime::addressList to;
    to.appendAddress(vmime::make_shared <vmime::mailbox>("vmimeteste@gmail.com"));
    mb.setRecipients(to);
    mb.setSubject(vmime::text("My first message generated with vmime::messageBuilder"));
    // Message body
    mb.getTextPart()->setText(vmime::make_shared <vmime::stringContentHandler>(
                                  "I'm writing this short text to test message construction " \
                                  "using the vmime::messageBuilder component."));
    // Construction
    vmime::shared_ptr <vmime::message> msg = mb.construct();

    // Raw text generation
    std::cout << "Generated message:" << std::endl;
    std::cout << "==================" << std::endl;

    vmime::utility::outputStreamAdapter out(std::cout);
    msg->generate(out);
    std::cout << "\n\n" << std::endl;

    vmime::utility::url url("smtp://smtp.gmail.com:587");
    vmime::shared_ptr<vmime::net::transport> tr = session->getTransport(url);
    tr->setProperty("connection.tls", true);
    tr->setProperty("auth.username", "vmimeteste@gmail.com");
    tr->setProperty("auth.password", "a1a2a3a4");
    tr->setProperty("options.need-authentication", true);
    tr->setCertificateVerifier(vmime::make_shared<SimpleCertificateVerifier>());
    tr->connect();
    tr->send(msg);

    std::cout << "------------" << std::endl;
}

int main(int , char**) {

    vmimeTest();
    return 0;

    cout << "jwSMTP library demo program\n\n";
    string to = "janosimas@gmail.com";
    string from = "janosimas@gmail.com";
    string smtpserver = "smtp.gmail.com";

    std::string subject = "Test";

    string mailmessage = "This is a test message";

    return 0;
}
