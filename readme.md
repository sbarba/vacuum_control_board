# Vacuum Control Board with Web Interface

This is an Arduino-based proof-of-concept for controlling the vacuum system of the JEOL JEM-2000FX
 transmission electron microscope. An Arduino board will be wired to the microscope's Pirani gauges and control valves. Mounted on the board will be an ethernet shield. The user will connect a computer to the shield with an ethernet cable and then manage the vacuum system via a web page found at a local IP address.

Specifically, the Arduino is connected to Pirani gauges and control valves on the microscope via analog and digital pins respectively. The Pirani gauge readings will be updated automatically at frequent intervals via Ajax. The control valves will be opened and closed with simple buttons that will reflect the state of the valves also via Ajax.