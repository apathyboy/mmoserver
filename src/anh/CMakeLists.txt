include(MMOServerLibrary)

AddMMOServerLibrary(libanh
    ADDITIONAL_SOURCE_DIRS
        database
        database/mysql
        event_dispatcher
)
