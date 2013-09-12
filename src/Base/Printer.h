//******************************************************************************
/*!
  \file      src/Base/Printer.h
  \author    J. Bakosi
  \date      Thu Sep 12 10:59:42 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Printer
  \details   Printer
*/
//******************************************************************************
#ifndef Printer_h
#define Printer_h

#include <string>
#include <iomanip>

#include <boost/format.hpp>

namespace quinoa {

//! Printer base
class Printer {

  public:
    //! Constructor
    explicit Printer() = default;

    //! Destructor
    virtual ~Printer() noexcept {}

    //! Print header
    void header(const std::string& title) const {
      std::cout << m_header_fmt % boost::io::group(std::setfill('='), "");
      std::cout << std::endl;
      std::cout << m_header_fmt % title;
      std::cout << std::endl;
      std::cout << m_header_fmt % boost::io::group(std::setfill('='), "");
    }

    //! Print part header: title
    void part(const std::string& title) const {
      std::cout << m_part_fmt % title;
    }

    //! Print section header: title
    void section(const std::string& title) const {
      std::cout << m_section_title_fmt % m_section_indent
                                       % m_section_bullet
                                       % title;
      std::cout << m_underline_fmt
                   % m_section_indent
                   % std::string(m_section_indent_size + 2 + title.size(),'-');
    }
    //! Print section header: title : value
    template<typename T>
    void section(const std::string& name, const T& value) const {
      std::cout << m_section_title_value_fmt % m_section_indent
                                             % m_section_bullet
                                             % name
                                             % value;
      std::cout << m_underline_fmt
                   % m_section_indent
                   % std::string(m_section_indent_size + 3 + name.size() +
                                 value.size(), '-');
    }

    //! Print subsection header: title
    void subsection(const std::string& title) const {
      std::cout << m_subsection_title_fmt % m_subsection_indent
                                          % m_subsection_bullet
                                          % title;
    }

    //! Print item: name : value
    template<typename T>
    void item(const std::string& name, const T& value) const {
      std::cout << m_item_name_value_fmt % m_item_indent % name % value;
    }
    //! Put std::endl in stream
    void endl() const { std::cout << std::endl; }

  protected:
    //! bullets
    const char m_section_bullet = '*';
    const char m_subsection_bullet = '<';
    //! indents
    const std::string m_section_indent = " ";
    const std::string m_subsection_indent = m_section_indent + "  ";
    const std::string m_item_indent = m_subsection_indent + "  ";
    //! indent sizes
    const std::string::size_type m_section_indent_size = m_section_indent.size();

    //! Format strings
    using format = boost::format;
    mutable format m_header_fmt = format("%|=80|\n");
    mutable format m_part_fmt = format("\n%|=80|\n");
    mutable format m_section_title_fmt = format("\n%s%c %s:\n");
    mutable format m_section_title_value_fmt = format("\n%s%c %s: %s\n");
    mutable format m_subsection_title_fmt = format("%s%c %s >\n");
    mutable format m_item_name_fmt = format("%s%-30s :");
    mutable format m_item_name_value_fmt = format("%s%-30s : %s\n");
    mutable format m_underline_fmt = format("%s%s\n");

  private:
    //! Don't permit copy constructor
    Printer(const Printer&) = delete;
    //! Don't permit copy assigment
    Printer& operator=(const Printer&) = delete;
    //! Don't permit move constructor
    Printer(Printer&&) = delete;
    //! Don't permit move assigment
    Printer& operator=(Printer&&) = delete;
};

} // namespace quinoa

#endif // Printer_h
