﻿//
//  Base64.h
//
//  Created by Collin B. Stuart on 2013-10-29.
//
//

#ifndef __CBS__Base64__
#define __CBS__Base64__

#include <stdio.h>
#include <string>

using namespace std;

class Base64
{
public:
    static std::string base64Encode(unsigned char const* , unsigned int length);
    static std::string base64Decode(std::string const& string);
    static size_t base64DecodeToData(const std::string& sourceString, void* destinationPointer, size_t destinationSize);
};

#include "MACROS.h"

namespace PPSHUAI
{
	namespace CRYPTO
	{
		struct Base64DecodeTables
		{
			signed char n[256];

			Base64DecodeTables()
			{
				for (int i = 0; i < 256; ++i)
				{
					n[i] = -1;
				}
				for (unsigned char i = '0'; i <= '9'; ++i)
				{
					n[i] = 52 + i - '0';
				}
				for (unsigned char i = 'A'; i <= 'Z'; ++i)
				{
					n[i] = i - 'A';
				}
				for (unsigned char i = 'a'; i <= 'z'; ++i)
				{
					n[i] = 26 + i - 'a';
				}
				n['+'] = 62;
				n['/'] = 63;
			}
			int operator [] (unsigned char i) const { return n[i]; }
		};

		static const std::string kBase64String = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		__inline static bool IsBase64(unsigned char c)
		{
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		class Base64
		{
		public:
			__inline static std::string base64Encode(unsigned char const * bytesToEncodeChar, unsigned int inLength)
			{
				std::string returnString;
				int i = 0;
				int j = 0;
				unsigned char characterArray3[3];
				unsigned char charArray4[4];

				while (inLength--)
				{
					characterArray3[i++] = *(bytesToEncodeChar++);
					if (i == 3)
					{
						charArray4[0] = (characterArray3[0] & 0xfc) >> 2;
						charArray4[1] = ((characterArray3[0] & 0x03) << 4) + ((characterArray3[1] & 0xf0) >> 4);
						charArray4[2] = ((characterArray3[1] & 0x0f) << 2) + ((characterArray3[2] & 0xc0) >> 6);
						charArray4[3] = characterArray3[2] & 0x3f;

						for (i = 0; (i < 4); i++)
						{
							returnString += kBase64String[charArray4[i]];
						}
						i = 0;
					}
				}

				if (i)
				{
					for (j = i; j < 3; j++)
					{
						characterArray3[j] = '\0';
					}

					charArray4[0] = (characterArray3[0] & 0xfc) >> 2;
					charArray4[1] = ((characterArray3[0] & 0x03) << 4) + ((characterArray3[1] & 0xf0) >> 4);
					charArray4[2] = ((characterArray3[1] & 0x0f) << 2) + ((characterArray3[2] & 0xc0) >> 6);
					charArray4[3] = characterArray3[2] & 0x3f;

					for (j = 0; (j < i + 1); j++)
					{
						returnString += kBase64String[charArray4[j]];
					}

					while ((i++ < 3))
					{
						returnString += '=';
					}

				}

				return returnString;

			}
			__inline static std::string base64Decode(std::string const & encodedString)
			{
				int inLength = encodedString.size();
				int i = 0;
				int j = 0;
				int in_ = 0;
				unsigned char charArray4[4], charArray3[3];
				std::string returnString;

				while (inLength-- && (encodedString[in_] != '=') && IsBase64(encodedString[in_]))
				{
					charArray4[i++] = encodedString[in_]; in_++;
					if (i == 4)
					{
						for (i = 0; i < 4; i++)
						{
							charArray4[i] = kBase64String.find(charArray4[i]);
						}

						charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
						charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
						charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

						for (i = 0; (i < 3); i++)
						{
							returnString += charArray3[i];
						}
						i = 0;
					}
				}

				if (i)
				{
					for (j = i; j < 4; j++)
					{
						charArray4[j] = 0;
					}

					for (j = 0; j < 4; j++)
					{
						charArray4[j] = kBase64String.find(charArray4[j]);
					}

					charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
					charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
					charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

					for (j = 0; (j < i - 1); j++)
					{
						returnString += charArray3[j];
					}
				}

				return returnString;
			}
			__inline static size_t base64DecodeToData(const std::string & sourceString, void* destinationPointer, size_t destinationSize)
			{
				static const Base64DecodeTables b64dt;
				if (!destinationSize)
				{
					return 0;
				}
				const size_t length = sourceString.length();
				int bc = 0, a = 0;
				char* const startPointer = static_cast<char*>(destinationPointer);
				char* pd = startPointer;
				char* const pend = pd + destinationSize;
				for (size_t i = 0; i < length; ++i)
				{
					const int n = b64dt[sourceString[i]];
					if (n == -1)
					{
						continue;
					}
					a |= (n & 63) << (18 - bc);
					if ((bc += 6) > 18)
					{
						*pd = a >> 16;
						if (++pd >= pend)
						{
							return pd - startPointer;
						}
						*pd = a >> 8;
						if (++pd >= pend)
						{
							return pd - startPointer;
						}
						*pd = a;
						if (++pd >= pend)
						{
							return pd - startPointer;
						}
						bc = a = 0;
					}
				}
				if (bc >= 8)
				{
					*pd = a >> 16;
					if (++pd >= pend)
					{
						return pd - startPointer;
					}
					if (bc >= 16)
					{
						*(pd++) = a >> 8;
					}
				}
				return pd - startPointer;
			}
		};
	}
}

#endif /* defined(__CBS__Base64__) */
