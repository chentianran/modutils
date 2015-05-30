#include "module.hh"

std::vector<std::string> Module::initialize_options (const std::vector<std::string>& opts)
{
    boost::program_options::parsed_options
        result = boost::program_options::basic_command_line_parser<char>(opts)
                    .options(options)
                    .allow_unregistered()
                    .run();
    boost::program_options::store  (result, parameters);
    boost::program_options::notify (parameters);
    return  boost::program_options::collect_unrecognized(
                result.options,
                boost::program_options::include_positional);
}
