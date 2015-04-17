#ifndef ne_impl_hpp
#define ne_impl_hpp

#include "log.hpp"
#include "memory_pool.hpp"
#include "message.hpp"
#include "config_manager.hpp"
#include "module_manager.hpp"
#include "global_object_manager.hpp"
#include "message_queue.hpp"
#include "ne.hpp"

class NEImpl : public NE {
public:
    NEImpl ();
    virtual ~NEImpl () {}

    /*!
     \brief create a new message from memory pool
     \param action_ the action of message
     \param station_ the current instance identifier
     \param attachment_ user's attachment need to deliver to another instance
     \param length_ the length of attachment
     \return created message pointer or null means failure
     */

    virtual void* createMessage (int action_, int station_,
        const void* attachment_, int length_);

    /*!
     \brief post a message to Noodle Engine
     \param message_ message
     \return OK or error code
     */

    virtual int postMessage (void* message_);

    /*!
     \brief destroy message
     \param message_ message pointer
     */

    virtual void destroyMessage (void* message_);

    /*!
     \brief get instance identifier of specific plugin
     \param pluginName_ plugin name
     \return instance identifier
     */

    virtual int getInstanceIdentifier (const char* pluginName_);

    /*!
     \brief set the current instance identifier
     \param message_ message pointer
     \param station_ the current instance identifier
     */

    virtual void setStation (void* message_, int station_);

    /*!
     \brief get the action of message
     \param message_ message pointer
     \return the action of message
     */

    virtual int getAction (void* message_);

    /*!
     \brief set the action of message
     \param message_ message pointer
     \param action_ the action of message
     */

    virtual void setAction (void* message_, int action_);

    /*!
     \brief get attachment of message
     \param message_ message pointer
     \return attachment of message
     */

    virtual void* getAttachment (void* message_);

    /*!
     \brief set attachment of message
     \param message_ message pointer
     \param attachment_ of message
     \param length_ the length of attachment
     */

    virtual void  setAttachment (void* message_, void* attachment_,
        int length_);

    /*!
     \brief get the attachment length of message
     \param message_ message pointer
     \return the length of attachment
     */

    virtual int getAttachmentLength (void* message_);

    /*!
     * \brief allocates memory from memory pool
     * \param size_ size
     * \return memory pointer, zero means fail
     */

    virtual void* alloc (int size_);

    /*!
     * \brief put memory to memory pool
     * \param size_ size
     * \param m_ memory pointer
     */

    virtual void free (void* m_, int size_);

    /*!
     \brief get integer value
     \param query_ query string
     \return integer value
     */

    virtual const int getInteger (const char* query_);

    /*!
     \brief get double value
     \param query_ query string
     \return double value
     */

    virtual const double getDouble (const char* query_);

    /*!
     \brief get C string value
     \param query_ query string
     \return C string value
     */

    virtual const char* getString (const char* query_);

    /*!
     \brief get boolean value
     \param query_ query string
     \return boolean value
     */

    virtual const bool getBoolean (const char* query_);

    /*!
     \brief get the length of array
     \param query_ query string
     \return the length of array
     */

    virtual const int getArrayLength (const char* query_);

    /*!
     \brief get the integer value of array by index
     \param query_ query string
     \param index_ array index
     \return the integer value
     */

    virtual const int getArrayIntegerByIndex (const char* query_,
        int index_);

    /*!
     \brief get the double value of array by index
     \param query_ query string
     \param index_ array index
     \return the double value
     */

    virtual const double getArrayDoubleByIndex (const char* query_,
        int index_);

    /*!
     \brief get the C string value of array by index
     \param query_ query string
     \param index_ array index
     \return the C string value
     */

    virtual const char* getArrayStringByIndex (const char* query_,
        int index_);

    /*!
     \brief get the boolean value of array by index
     \param query_ query string
     \param index_ array index
     \return the boolean value
     */

    virtual const bool getArrayBooleanByIndex (const char* query_,
        int index_);

    /*!
     \brief get the length of dictionary
     \param query_ query string
     \return the length of dictionary
     */

    virtual const int getDictLength (const char* query_);

    /*!
     \brief get the integer key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return integer key value
     */

    virtual const int getDictIntegerKeyByIndex (const char* query_,
        int index_);

    /*!
     \brief get the double key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return double key value
     */

    virtual const double getDictDoubleKeyByIndex (const char* query_,
        int index_);

    /*!
     \brief get the string key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return C string key value
     */

    virtual const char* getDictStringKeyByIndex (const char* query_,
        int index_);

    /*!
     \brief get the boolean key by index in dictionary
     \param query_ query string
     \param index_ index in dictionary
     \return boolean key value
     */

    virtual const bool getDictBooleanKeyByIndex (const char* query_,
        int index_);

    /*!
     \brief write log
     \param level_ log level
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, const char* fmt_, ...);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, int prefix_, const char* fmt_, ...);

    /*!
     \brief write log
     \param level_ log level
     \param prefix_ the prefix of log line
     \param file_ file name
     \param func_ function name
     \param line_ line number
     \param fmt_ fomatted string like fprintf
     */

    virtual void writeLog (int level_, int prefix_, const char* file_,
        const char* func_, int line_, const char* fmt_, ...);

private:
    Log*           _log;           ///< log
    MemoryPool*    _memoryPool;    ///< memory pool
    ConfigManager* _configManager; ///< config manager
    ModuleManager* _moduleManager; ///< module manager
    MessageQueue*  _coreMessageQueue; ///< core message queue
};

#endif // ne_impl_hpp
