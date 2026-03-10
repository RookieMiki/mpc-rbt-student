from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    return LaunchDescription([
        # Spuštění naprogramovaného Nodu i s parametry
        Node(
            package='my_first_package',
            executable='my_publisher',
            name='muj_super_node',
            parameters=[{
                'battery_max_voltage': 42.0,
                'battery_min_voltage': 36.0
            }]
        ),
        
        # Spuštění nahrávání dat (ROS 2 Bag)
        # Nahráváme topic /battery_voltage a /battery_percentage do složky 'battery_bag'
        ExecuteProcess(
            cmd=['ros2', 'bag', 'record', '-o', 'battery_bag', '/battery_voltage', '/battery_percentage'],
            output='screen'
        )
    ])
