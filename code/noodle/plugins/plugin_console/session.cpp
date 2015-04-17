/*
 * Copyright (c) 2009-2010, Dennis Wang (dennis.wangkk@gmail.com)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>

#include "plugin_console.hpp"
#include "session.hpp"

Session::Session (Server* server_, NE* environment_,
    PluginConsole* console_,
    boost::asio::io_service& io_service_)
    : _server (server_),
      _environment (environment_),
      _console (console_),
      _socket (io_service_),
      _rbufLength (0) {
    memset (_rbuf, 0, MAX_READ_BUFFER_LENGTH);
    memset (_wbuf, 0, MAX_WRITE_BUFFER_LENGTH);
}

void Session::start() {
    _socket.async_read_some(
        boost::asio::buffer(_rbuf, MAX_READ_BUFFER_LENGTH),
        boost::bind(
            &Session::read,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void Session::read (const boost::system::error_code& error,
    size_t bytes) {
    if (!error) {
        _rbufLength += bytes;
        if (!_verifyCompletion (_rbuf, _rbufLength)) {
            _socket.async_read_some(
                boost::asio::buffer(
                    _rbuf + _rbufLength,
                    MAX_READ_BUFFER_LENGTH - _rbufLength),
                boost::bind(
                    &Session::read,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        } else {
            try {
                if (!_execute (_rbuf)) {
                    _writeMessage ("unknown command");
                }
            } catch (std::exception& e) {
                _writeMessage (e.what ());
            }            
        }
    } else {
        delete this;
    }
}

void Session::write (const boost::system::error_code& error) {
    _rbufLength = 0;
    memset (_rbuf, 0, MAX_READ_BUFFER_LENGTH);

    if (!error) {
        _socket.async_read_some(
            boost::asio::buffer(_rbuf, MAX_READ_BUFFER_LENGTH),
            boost::bind(
                &Session::read,
                this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    } else {
        delete this;
    }
}

void Session::_writeMessage (const char* message_) {
    assert (message_);

    size_t bufLength = strlen (message_);

    memcpy (_wbuf, message_, bufLength);
    _wbuf[bufLength]     = '\r';
    _wbuf[bufLength + 1] = '\n';
    bufLength            += 2;

    boost::asio::async_write(
        _socket,
        boost::asio::buffer(_wbuf, bufLength),
            boost::bind(
                &Session::write,
                this,
                boost::asio::placeholders::error));
}

bool Session::_verifyCompletion (char* buf, size_t length_) {
    if (length_ <= 1) {
        return false;
    }

    return ((buf[length_ - 1] == '\n') && (buf[length_ - 2] == '\r'))
        ? true : false;
}

using namespace boost::algorithm;

bool Session::_execute (char* cmd_) {
    std::string cmdline (cmd_);
    split_vector_type splitVec;
    split (splitVec, cmdline, is_any_of (" \t\r\n"), token_compress_on);

    if (splitVec.empty ()) {
        return false;
    }

    if (splitVec[0] == "stop") {
        return _onStop (splitVec);
    } else if (splitVec[0] == "start") {
        return _onStart (splitVec);
    } else if (splitVec[0] == "enable") {
        return _onEnable (splitVec);
    } else if (splitVec[0] == "disable") {
        return _onDisable (splitVec);
    } else if (splitVec[0] == "install") {
        return _onInstall (splitVec);
    } else if (splitVec[0] == "uninstall") {
        return _onUninstall (splitVec);
    } else if (splitVec[0] == "help") {
        return _onHelp (splitVec);
    } else if (splitVec[0] == "update") {
        return _onUpdate (splitVec);
    } else if (splitVec[0] == "shutdown") {
        return _onShutdown (splitVec);
    } else if (splitVec[0] == "stat") {
        return _onStat (splitVec);
    }

    return false;
}

void Session::_postSimpleCoreMessage (Command* command_) {
    int   id      = _environment->getInstanceIdentifier ("console");
    void* message = _environment->createMessage (command_->msgID, id,
        command_, sizeof (Command));

    _console->putMessage (message, true);
    _console->wait ();
    _writeMessage (_console->getResult ().c_str ());
}

using namespace boost::program_options;

bool Session::_onHelp (split_vector_type& splitCmd_) {
    options_description stopDesc("stop");

    stopDesc.add_options()
    ("instance,i", value<int>(), "instance identifier")
    ;

    options_description startDesc("start");

    startDesc.add_options()
    ("instance,i", value<int>(), "instance identifier")
    ("module,m", value<std::string>(), "plugin name")
    ;

    options_description enableDesc("enable");

    enableDesc.add_options()
    ("instance,i", value<int>(), "instance identifier")
    ;

    options_description disableDesc("disable");

    disableDesc.add_options()
    ("instance,i", value<int>(), "instance identifier")
    ;

    options_description installDesc("install");

    installDesc.add_options()
    ("module,m", value<std::string>(), "plugin name")
    ("path,p",   value<std::string>(), "plugin path")
    ("source,s", "is a source")
    ;

    options_description uninstallDesc("uninstall");

    uninstallDesc.add_options()
    ("module,m", value<std::string>(), "plugin name")
    ;

    options_description statDesc("stat");

    statDesc.add_options()
    ("tree,t", "print the tree of plugin")
    ("config,c", "print config file content")
    ("message,m", "print message has been processed")
    ("version,v", "print version number of core")
    ("pool,p", "print the statistics info of memory pool")
    ;

    options_description updateDesc("update");

    updateDesc.add_options()
    ("old,o", value<std::string>(), "old plugin name")
    ("new,n", value<std::string>(), "new plugin name")
    ("path,p", value<std::string>(), "new plugin path")
    ;

    options_description desc ("Allowed Commands");
    desc.add (startDesc).add (stopDesc).add (enableDesc).add (
        disableDesc).add (installDesc).add (uninstallDesc).add (
        updateDesc).add (statDesc);

    std::stringstream sstream;
    sstream << desc;

    _writeMessage (sstream.str ().c_str ());
    return true;
}

bool Session::_onStop (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("instance,i", value<int>(), "stop instance")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    int instanceID = 0;
    if (vm.count ("instance")) {
        instanceID = vm["instance"].as<int>();

        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID               = MSG_DESTROY;
        command.cmdBasic.instanceID = instanceID;

        _postSimpleCoreMessage (&command);
    } else {
        throw std::runtime_error ("need a instance identifier");
    }

    return true;
}

bool Session::_onStart (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("instance,i", value<int>(), "start instance")
    ("module,m", value<std::string>(), "plugin name")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    int instanceID = 0;
    if (vm.count ("instance")) {
        instanceID = vm["instance"].as<int>();
    } else {
        throw std::runtime_error ("need a instance identifier");
    }

    std::string pluginName;
    if (vm.count ("module")) {
        pluginName = vm["module"].as<std::string>();
    } else {
        throw std::runtime_error ("need a module name");
    }

    Command command;
    memset (&command, 0, sizeof (Command));

    command.msgID               = MSG_CREATE;
    command.cmdStart.instanceID = instanceID;

    memcpy (command.cmdStart.pluginName, pluginName.c_str (),
        pluginName.size () + 1);

    _postSimpleCoreMessage (&command);

    return true;
}

bool Session::_onEnable (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("instance,i", value<int>(), "enable instance")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    int instanceID = 0;
    if (vm.count ("instance")) {
        instanceID = vm["instance"].as<int>();

        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID               = MSG_ENABLE;
        command.cmdBasic.instanceID = instanceID;

        _postSimpleCoreMessage (&command);
    } else {
        throw std::runtime_error ("need a instance identifier");
    }

    return true;
}

bool Session::_onDisable (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("instance,i", value<int>(), "disable instance")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    int instanceID = 0;
    if (vm.count ("instance")) {
        instanceID = vm["instance"].as<int>();

        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID               = MSG_DISABLE;
        command.cmdBasic.instanceID = instanceID;

        _postSimpleCoreMessage (&command);
    } else {
        throw std::runtime_error ("need a instance identifier");
    }

    return true;
}

bool Session::_onInstall (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("module,m", value<std::string>(), "module name")
    ("path,p",   value<std::string>(), "module path")
    ("source,s", "is a source")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    std::string moduleName;
    std::string modulePath;
    bool source = false;

    if (vm.count ("module")) {
        moduleName = vm["module"].as<std::string>();
    } else {
        throw std::runtime_error ("need a module name");
    }

    if (vm.count ("path")) {
        modulePath = vm["path"].as<std::string>();
    } else {
        throw std::runtime_error ("need a module path");
    }

    if (vm.count ("source")) {
       source = true;
    }

    Command command;
    memset (&command, 0, sizeof (Command));

    command.msgID             = MSG_INSTALL;
    command.cmdInstall.source = source;

    memcpy (command.cmdInstall.pluginName, moduleName.c_str (),
        moduleName.size () + 1);

    memcpy (command.cmdInstall.pluginPath, modulePath.c_str (),
        modulePath.size () + 1);

    _postSimpleCoreMessage (&command);

    return true;
}

bool Session::_onUninstall (split_vector_type& splitCmd_) {
    options_description desc("command options");

    desc.add_options()
    ("module,m", value<std::string>(), "module name")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    std::string moduleName;
    std::string modulePath;
    bool source = false;

    if (vm.count ("module")) {
        moduleName = vm["module"].as<std::string>();
    } else {
        throw std::runtime_error ("need a module name");
    }

    Command command;
    memset (&command, 0, sizeof (Command));

    command.msgID = MSG_UNINSTALL;

    memcpy (command.cmdUninstall.pluginName, moduleName.c_str (),
        moduleName.size () + 1);

    _postSimpleCoreMessage (&command);

    return true;
}

bool Session::_onUpdate (split_vector_type& splitCmd_) {
    options_description desc("update");

    desc.add_options()
    ("old,o", value<std::string>(), "old plugin name")
    ("new,n", value<std::string>(), "new plugin name")
    ("path,p", value<std::string>(), "new plugin path")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    std::string oldPluginName;
    std::string newPluginName;
    std::string path;

    if (vm.count ("old")) {
        oldPluginName = vm["old"].as<std::string>();
    } else {
        throw std::runtime_error ("need old module name");
    }

    if (vm.count ("new")) {
        newPluginName = vm["new"].as<std::string>();
    } else {
        throw std::runtime_error ("need new module name");
    }

    if (vm.count ("path")) {
        path = vm["path"].as<std::string>();
    } else {
        throw std::runtime_error ("need module path");
    }

    Command command;
    memset (&command, 0, sizeof (Command));

    command.msgID = MSG_UPDATE;

    memcpy (command.cmdUpdate.oldPluginName,
        oldPluginName.c_str (), oldPluginName.size () + 1);

    memcpy (command.cmdUpdate.newPluginName,
        newPluginName.c_str (), newPluginName.size () + 1);

    memcpy (command.cmdUpdate.pluginPath,
        path.c_str (), path.size () + 1);

    _postSimpleCoreMessage (&command);

    return true;
}

bool Session::_onShutdown (split_vector_type& splitCmd_) {
    Command command;
    memset (&command, 0, sizeof (Command));

    command.msgID = MSG_SHUTDOWN;
    _postSimpleCoreMessage (&command);

    return true;
}

bool Session::_onStat (split_vector_type& splitCmd_) {
    options_description desc("stat");

    desc.add_options()
    ("tree,t", "print the tree of plugin")
    ("config,c", "print config file content")
    ("message,m", "print message has been processed")
    ("version,v", "print version number of core")
    ("pool,p", "print the statistics info of memory pool")
    ;

    // parse program options

    variables_map vm;
    try {
        store(command_line_parser (splitCmd_).options (desc).run (), vm);
        notify(vm);
    } catch (std::exception& e) {
        (void) e;
        throw std::runtime_error ("invalid command format");
    }

    bool printTree = false;
    bool printConfig = false;
    bool printMessage = false;
    bool printVersion = false;
    bool printPool = false;

    if (vm.count ("tree")) {
        printTree = true;
    }

    if (vm.count ("config")) {
        printConfig = true;
    }

    if (vm.count ("message")) {
        printMessage = true;
    }

    if (vm.count ("version")) {
        printVersion = true;
    }

    if (vm.count ("pool")) {
        printPool = true;
    }

    if (printTree) {
        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID = MSG_STAT;
        command.cmdStat.printTree = true;
        _postSimpleCoreMessage (&command);
    } else if (printConfig) {
        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID = MSG_STAT;
        command.cmdStat.printConfig = true;
        _postSimpleCoreMessage (&command);
    } else if (printMessage) {
        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID = MSG_STAT;
        command.cmdStat.printMessage = true;
        _postSimpleCoreMessage (&command);
    } else if (printVersion) {
        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID = MSG_STAT;
        command.cmdStat.printVersion = true;
        _postSimpleCoreMessage (&command);
    } else if (printPool) {
        Command command;
        memset (&command, 0, sizeof (Command));

        command.msgID = MSG_STAT;
        command.cmdStat.printPool = true;
        _postSimpleCoreMessage (&command);
    } else {
        _writeMessage ("need options");
    }

    return true;
}
