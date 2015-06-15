#ifndef _MODUTILS_HH_
#define _MODUTILS_HH_

#include <string>
#include <vector>
#include <boost/program_options.hpp>

/// Module is the super class for all "modules".
/// A module is an independent and replaceable component.
/// This super class also implements a module recipe system
/// which allows a module to get options in a uniform way.
class Module
{
public:

    virtual ~Module () { }

    /// Returns the name of the module.
    /// This function returns the short name of the module
    /// which uniquely identifies the module.
    virtual const char* name () const = 0;

    /// Returns the detailed description of the module.
    virtual const char* description () const { return 0; }


    boost::program_options::options_description options;
    boost::program_options::variables_map       parameters;

protected:

    std::vector<std::string> initialize_options (const std::vector<std::string>&);

    template <typename T>
    T get_opt (const std::string& opt)
    {
        return parameters[std::string(name()) + "." + opt].as<T>();
    }
};

#define MODULE_DCL                                          \
    const char* name ()        const;                       \
    const char* description () const;

#define MODULE_DEF(CLASS,SUPER,NAME,DESC)                   \
    struct CLASS##Factory : public SUPER::Factory {         \
        CLASS##Factory() { SUPER::create(NAME,this); }      \
        SUPER* create() override { return new CLASS(); }    \
    };                                                      \
    static CLASS##Factory global_##CLASS##Factory;          \
    const char* CLASS::name       () const { return NAME; } \
    const char* CLASS::description() const { return DESC; }

#define MODULE_TYPE(CLASS)                                  \
    struct Factory { virtual CLASS* create() = 0; };        \
    static CLASS* create (                                  \
        const std::string& name,                            \
        CLASS::Factory* factory,                            \
        std::vector<std::string>* listing = 0)              \
    {                                                       \
        typedef std::map<std::string,CLASS::Factory*> FMAP; \
        static FMAP factories;                              \
        if (factory) {                                      \
            factories[name] = factory;                      \
            return 0;                                       \
        } else if (listing) {                               \
            for (auto f : factories)                        \
                listing->push_back (f.first);               \
	    return 0;                                       \
        } else {                                            \
            FMAP::iterator iter = factories.find (name);    \
            if (factories.end() == iter)                    \
                throw "cannot find module";                 \
            return iter->second->create();                  \
        }                                                   \
    }                                                       \
    static CLASS* create (                                  \
        const std::string& name,                            \
        const std::vector<std::string>& opts                \
    ) {                                                     \
        CLASS* obj = create (name, 0);                      \
        obj->initialize_options (opts);                     \
        return obj;                                         \
    }                                                       \
    static std::vector<std::string> listing() {             \
        std::vector<std::string> lst;                       \
        create (std::string(), 0, &lst);                    \
        return lst;                                         \
    }

#define MODULE_OPT(NAME,TYPE,DEFAULT,DESC)                  \
    ( (std::string(this->name()) + "." + NAME).c_str(),     \
      boost::program_options::value<TYPE>()                 \
      ->default_value(DEFAULT),                             \
      DESC )

#endif
