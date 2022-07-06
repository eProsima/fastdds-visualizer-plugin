.. include:: ../../../exports/alias.include
.. include:: ../../../exports/roles.include

.. _cmake_options:

#############
CMake options
#############

|ddsvisualizer| provides several CMake options to build the documentation and tests.

.. warning::

    These options are only for developers who installed the |ddsvisualizer| following
    the compilation steps described in :ref:`developer_manual_installation_sources_linux`.

.. list-table::
    :header-rows: 1

    *   - Option
        - Description
        - Possible values
        - Default

    *   - :class:`CMAKE_BUILD_TYPE`
        - Type of build (optional). Setting :class:`CMAKE_BUILD_TYPE` |br|
          to ``Debug`` enables debugging and console logging.
        - ``Debug`` |br|
          ``Release``
        - -

    *   - :class:`BUILD_TESTS`
        - Build the |ddsvisualizer| application |br|
          and documentation tests. Setting :class:`BUILD_TESTS` to ``ON`` sets |br|
          :class:`BUILD_DOCUMENTATION_TESTS` to ``ON``.
        - ``OFF`` |br|
          ``ON``
        - ``OFF``

    *   - :class:`BUILD_DOCUMENTATION_TESTS`
        - Build the |ddsvisualizer| documentation tests. |br|
          It is set to ``ON`` if :class:`BUILD_TESTS` is set to ``ON``.
        - ``OFF`` |br|
          ``ON``
        - ``OFF``

    *   - :class:`BUILD_DOCUMENTATION`
        - Build the |ddsvisualizer| documentation. |br|
          It is set to ``ON`` if :class:`BUILD_TESTS_DOCUMENTATION` |br|
          is set to ``ON``.
        - ``OFF`` |br|
          ``ON``
        - ``OFF``
