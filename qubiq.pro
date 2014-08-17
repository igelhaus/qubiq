#-------------------------------------------------
#
# Project created by QtCreator 2014-03-05T09:10:37
#
#-------------------------------------------------

#
# All subprojects (including 3rd party components and tests):
#

TEMPLATE = subdirs
SUBDIRS  = core et \
    3rdparty/cutelogger        \
    tests/test_lexeme          \
    tests/test_text            \
    tests/test_lexeme_sequence \
    tests/test_extractor

#
# Core dependencies:
#

cutelogger.depends =
core.depends       = 3rdparty/cutelogger
et.depends         = core

#
# Test dependencies:
#

test_lexeme.depends          = core
test_text.depends            = core
test_lexeme_sequence.depends = core
test_extractor.depends       = core
