// version.h
#ifndef VERSION_H
#define VERSION_H

// Version numbers
#define VERSION_MAJOR 1
#define VERSION_MINOR 4
#define VERSION_PATCH 0

// Helper macros to convert numbers to strings
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Create a version string
#define VERSION_STRING " 1C Kafka / MDM " TOSTRING(VERSION_MAJOR) "." TOSTRING(VERSION_MINOR) "." TOSTRING(VERSION_PATCH)

#endif // VERSION_H