/*
 This file is part of MMOServer. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2010 The SWG:ANH Team

 MMOServer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MMOServer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MMOServer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANH_CONVERTABLE_STRING_H_
#define ANH_CONVERTABLE_STRING_H_

#include <algorithm>
#include <string>

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

namespace anh {

class ConvertableString {
public:
    /// Takes a human readable string and stores a hash of it.
    ConvertableString(const std::string& string)
        : string_(string) {}

    /// Default destructor.
    ~ConvertableString() {}

    /// Copy constructor.
    ConvertableString(const ConvertableString& other) {
        string_ = other.string_;
    }

    /// Move constructor.
    ConvertableString(ConvertableString&& other) {
        string_ = std::move(other.string_);
    }
    
    /// Swap the contents of two HashStrings.
    void swap(ConvertableString& other) {
        std::swap(string_, other.string_);
    }

    /// Universal assignment operator.
    ConvertableString& operator=(ConvertableString other) {
        other.swap(*this);
        return *this;
    }

    template<typename T>
    boost::optional<T> as() const {
        boost::optional<T> ret;

        try {
            ret.reset(boost::lexical_cast<T>(string_));
        } catch(...) {}

        return ret;
    }

private:
    std::string string_;
};

}  // namespace anh

#endif  // ANH_CONVERTABLE_STRING_H_
