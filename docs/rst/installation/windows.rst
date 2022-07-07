.. include:: ../exports/alias.include

.. _installation_manual_windows:

#####################################
Fast DDS Visualizer Plugin on Windows
#####################################

This section provides instructions on how to install the |ddsvisualizer| application.
This is available on the `eProsima <https://www.eprosima.com/>`_ website in the
`Downloads <https://www.eprosima.com/index.php/downloads-all>`_ section.

In the list of eProsima official releases, search for |ddsvisualizer| and go to the available files of
the latest version available. Then click on the **Download now** button of the Windows installer
(``PlotJuggler eProsima Edition x.x.x - Win (32 & 64)`` where ``X.X.X`` is the version of the application.).

Now locate the downloaded file and run the installer. The |ddsvisualizer| installer window should open as shown
in the following image.

.. .. figure:: /rst/figures/windows_installer.png
..     :align: center


Follow the installation steps until the installation process is complete.
A window as the one shown below should be visible on screen after installation, from which you may
directly run the |ddsvisualizer| application.

.. .. figure:: /rst/figures/windows_installer_run.png
..     :align: center


OpenGL Troubleshooting
======================

At application startup, if a black screen like the following image appears:

.. .. figure:: /rst/figures/black_screen_windows.png
..     :align: center


it is likely because an incompatibility between the OpenGL version used in
PlotJuggler and the latest versions of Windows. To solve this problem, go to Preferences:

.. .. figure:: /rst/figures/black_screen_windows_preferences.png
..     :align: center


and then disable the OpenGL checkbox:

.. .. figure:: /rst/figures/black_screen_windows_disable_opengl.png
..     :align: center


After clicking OK and **restarting** the application, the problem should now be solved.
