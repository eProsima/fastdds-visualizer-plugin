.. include:: ../../../exports/alias.include
.. include:: ../../../exports/roles.include

.. _cmake_options:

#############
CMake options
#############

*eProsima DDS Visualizer Plugin* provides several CMake options to build the documentation and tests.

.. warning::
    These options are only for developers who installed the *DDS Visualizer Plugin* following 
    the compilation steps described in :ref:`developer_manual_installation_sources_linux`.

.. list-table::
    :header-rows: 1

    *   - Option
        - Description
        - Possible values
        - Default
    *   - :class:`BUILD_TESTS`
        - Build the *DDS Visualizer Plugin* application and documentation |br|
          tests. Setting :class:`BUILD_TESTS` to ``ON`` sets |br|
          :class:`BUILD_DOCUMENTATION_TESTS` to ``ON``.
        - ``OFF`` |br|
          ``ON``
        - ``OFF``
    *   - :class:`BUILD_DOCUMENTATION_TESTS`
        - Build the *DDS Visualizer Plugin* documentation tests. It is |br|
          set to ``ON`` if :class:`BUILD_TESTS` is set to ``ON``. |br|
        - ``OFF`` |br|
          ``ON``
        - ``OFF``
    *   - :class:`BUILD_DOCUMENTATION`
        - Build the *DDS Visualizer Plugin* documentation. It is |br|
          set to ``ON`` if :class:`BUILD_TESTS_DOCUMENTATION` is set |br|
          to ``ON``.
        - ``OFF`` |br|
          ``ON``
        - ``OFF``
