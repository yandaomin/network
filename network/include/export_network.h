#ifndef EXPORT_NETWORK_H
#define EXPORT_NETWORK_H

#	ifdef NETWORK_EXPORTS
#		if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#			define NETWORK_EXPORT __declspec(dllexport)
#		else
#			define NETWORK_EXPORT
#		endif
#	else
#		if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#			define NETWORK_EXPORT __declspec(dllimport)
#		else
#			define NETWORK_EXPORT
#		endif
#	endif


#endif