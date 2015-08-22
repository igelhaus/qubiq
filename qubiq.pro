#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T09:10:37
#
#-------------------------------------------------

#
# All subprojects (including 3rd party components and tests):
#

TEMPLATE = subdirs
SUBDIRS  = core util et btd \
    3rdparty/cutelogger        \
    tests/test_lexeme          \
    tests/test_lexeme_index    \
    tests/test_lexeme_sequence \
    tests/test_text            \
    tests/test_extractor       \
    tests/test_transducer

#
# Core dependencies:
#

cutelogger.depends =
util.depends       =
core.depends       = util 3rdparty/cutelogger
et.depends         = core

#
# Test dependencies:
#

test_lexeme.depends          = util
test_lexeme_index.depends    = util
test_lexeme_sequence.depends = core
test_text.depends            = core
test_extractor.depends       = core
