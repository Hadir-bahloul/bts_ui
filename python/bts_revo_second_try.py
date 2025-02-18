import tkinter as tk

# Track the state of Dagger and Blitz extensions
dagger_states = [False] * 6  # 6 Daggers, initially off
blitz_states = [False] * 3    # 3 Blitz modules, initially off

# Store initial colors for Dagger, Sword, and Blitz buttons
initial_colors = {
    "dagger": "#FDD835",  # Dagger/Spark color
    "sword": "#64B5F6",   # Sword color
    "blitz": "#64B5F6",   # Blitz color
}

def toggle_extension(button, button_type, linked_buttons=[]):
    """Toggle the color of the extension button and linked buttons"""
    current_color = button.cget("bg")
    new_color = "green" if current_color == initial_colors[button_type] else initial_colors[button_type]
    button.config(bg=new_color)
    
    # Toggle the linked buttons' colors
    for linked_button in linked_buttons:
        linked_button.config(bg=new_color)

def toggle_channels(start_channel, num_channels):
    """Toggle a sequence of channels from start_channel for num_channels"""
    for i in range(num_channels):
        channel_number = start_channel + i
        channel_label = channel_labels[channel_number - 1]  # Adjust for 0-based index
        current_color = channel_label.cget("bg")
        new_color = "green" if current_color == "gray" else "gray"
        channel_label.config(bg=new_color)

# Initialize the main window
root = tk.Tk()
root.title("BTS Revo Layout")
root.geometry("1200x900")  # Adjusted window size to accommodate the right-side functions

# Color scheme
dagger_spark_color = "#FDD835"
#sword_extension_color = "#E57373"
blitz_extension_color = "#64B5F6"
number_box_color = "#4CAF50"
function_button_color = "#81C784"  # Color for function buttons

# Create a frame to hold the left side (extensions) and right side (functions)
left_frame = tk.Frame(root, bg="#f0f0f0", padx=10, pady=10)
left_frame.grid(row=0, column=0, sticky="nsew")

right_frame = tk.Frame(root, bg="#f0f0f0", padx=10, pady=10)
right_frame.grid(row=0, column=1, sticky="nsew")

# Create a function for adding the base functions
def add_base_functions():
    base_functions = [
        "Open Load", "Short Vbat", "Short GND", "PWM Generation", "Low Current Generation", 
        "Voltage Measurement", "Current Measurement (Low Current)", "PWM Measurement"
    ]
    
    # Add the base functions to the right frame
    for i, func in enumerate(base_functions):
        button = tk.Button(right_frame, text=func, bg=function_button_color, width=20, height=2, relief="raised", bd=2)
        button.grid(row=i, column=0, padx=10, pady=5, sticky="ew")

# Create Dagger/Spark Extensions (Each controls 4 channels)
def enable_dagger_functions():
    """Enable Dagger-specific functions and show related channel numbers"""
    global dagger_function_button
    active_channels = []
    for i, state in enumerate(dagger_states):
        if state:
            start_channel = i * 4 + 1
            end_channel = start_channel + 3
            active_channels.append(f"{start_channel}-{end_channel}")
    
    if active_channels:  # Only enable if there are active channels
        channel_text = "Channels " + ", ".join(active_channels)
        # Clear existing button if any
        disable_dagger_functions()
        # Create new button
        dagger_function_button = tk.Button(
            right_frame, 
            text=f"Load Simulation (Resistor)\n{channel_text}", 
            bg=function_button_color, 
            width=20, 
            height=3,  # Increased height to accommodate additional text
            relief="raised", 
            bd=2
        )
        dagger_function_button.grid(row=8, column=0, padx=10, pady=5, sticky="ew")
    else:
        disable_dagger_functions()  # Disable if no active channels

def disable_dagger_functions():
    """Disable Dagger-specific functions"""
    global dagger_function_button
    if dagger_function_button:
        dagger_function_button.destroy()
        dagger_function_button = None

def toggle_dagger_extension(dagger_index):
    global dagger_states
    dagger_states[dagger_index] = not dagger_states[dagger_index]  # Toggle state
    dagger_button = left_frame.grid_slaves(row=0, column=dagger_index)[0]  # Find the Dagger button
    toggle_extension(dagger_button, "dagger")
    
    # Change color of 4 channels corresponding to this Dagger
    toggle_channels(dagger_index * 4 + 1, 4)
    
    # Enable or disable Dagger-specific function based on state
    if any(dagger_states):  # If any Dagger is active
        enable_dagger_functions()
    else:
        disable_dagger_functions()

# Create "BTS Revo" Label in the middle
tk.Label(left_frame, text="BTS Revo", font=("Arial", 14, "bold"), width=20, height=2, bg="#f0f0f0").grid(row=1, column=2, columnspan=2, pady=20)

# Create Sword and Blitz Extensions (Each Blitz with 2 Swords controls 8 channels)
blitz_sword_pairs = []  # List to store Blitz and its corresponding swords

def enable_blitz_functions():
    """Enable Blitz-specific functions and show related channel numbers"""
    global blitz_function_buttons
    active_channels = []
    for i, state in enumerate(blitz_states):
        if state:
            start_channel = i * 8 + 1
            end_channel = start_channel + 7
            active_channels.append(f"{start_channel}-{end_channel}")
    
    if active_channels:  # Only enable if there are active channels
        channel_text = "Channels " + ", ".join(active_channels)
        # Clear existing buttons if any
        disable_blitz_functions()
        # Create new buttons
        blitz_functions = [
            f"Current Measurement (High Current)\n{channel_text}", 
            f"Quiescent Current Measurement\n{channel_text}", 
            f"Short to GND (High Current)\n{channel_text}", 
            f"Load Simulation (High Current)\n{channel_text}"
        ]
        
        blitz_function_buttons = []
        for i, func in enumerate(blitz_functions):
            button = tk.Button(
                right_frame, 
                text=func, 
                bg=function_button_color, 
                width=20, 
                height=3,  # Increased height to accommodate additional text
                relief="raised", 
                bd=2
            )
            button.grid(row=8 + i, column=0, padx=10, pady=5, sticky="ew")
            blitz_function_buttons.append(button)
    else:
        disable_blitz_functions()  # Disable if no active channels

def disable_blitz_functions():
    """Disable Blitz-specific functions"""
    global blitz_function_buttons
    if blitz_function_buttons:
        for button in blitz_function_buttons:
            button.destroy()
        blitz_function_buttons = []

def toggle_blitz_and_swords(blitz_index):
    global blitz_states
    blitz_states[blitz_index] = not blitz_states[blitz_index]  # Toggle state
    blitz_button = blitz_sword_pairs[blitz_index][0]  # Get the Blitz button
    sword1_button = blitz_sword_pairs[blitz_index][1]  # Get the Sword 1 button
    sword2_button = blitz_sword_pairs[blitz_index][2]  # Get the Sword 2 button
    
    # Toggle Blitz and its Sword buttons
    toggle_extension(blitz_button, "blitz", linked_buttons=[sword1_button, sword2_button])
    
    # Change color of 8 channels corresponding to this Blitz
    toggle_channels(blitz_index * 8 + 1, 8)
    
    # Enable or disable Blitz-specific functions based on state
    if any(blitz_states):  # If any Blitz is active
        enable_blitz_functions()
    else:
        disable_blitz_functions()

# Initialize Dagger and Blitz function buttons
dagger_function_button = None
blitz_function_buttons = []

# Create Dagger buttons
for i in range(6):  # 6 Daggers
    btn = tk.Button(left_frame, text="Dagger/Spark\nExtension", bg=dagger_spark_color, width=15, height=3, relief="raised", bd=2)
    btn.config(command=lambda i=i: toggle_dagger_extension(i))
    btn.grid(row=0, column=i, padx=10, pady=10, sticky="ew")

# Create Blitz and Sword buttons
for i in range(3):  # 3 Blitz modules
    sword1 = tk.Button(left_frame, text="Sword\nExtension", bg=blitz_extension_color, width=15, height=3, relief="raised", bd=2)
    sword2 = tk.Button(left_frame, text="Sword\nExtension", bg=blitz_extension_color, width=15, height=3, relief="raised", bd=2)
    blitz = tk.Button(left_frame, text="Blitz\nExtension", bg=blitz_extension_color, width=32, height=3, relief="raised", bd=2)  # Covers 2 columns

    blitz_sword_pairs.append((blitz, sword1, sword2))

    sword1.grid(row=2, column=i * 2, padx=5, pady=5, sticky="ew")
    sword2.grid(row=2, column=i * 2 + 1, padx=5, pady=5, sticky="ew")
    blitz.grid(row=3, column=i * 2, columnspan=2, padx=5, pady=5, sticky="ew")

    blitz.config(command=lambda i=i: toggle_blitz_and_swords(i))
    
# Create Numbered Boxes at the bottom (24 channels)
channel_labels = []  # To store references to channel labels for toggling
for i in range(6):  # 6 rows
    for j in range(4):  # 4 channels in each row
        label = tk.Label(left_frame, text=str(i * 4 + j + 1), bg=number_box_color, fg="white", width=5, height=2, relief="raised", bd=2)
        label.grid(row=4 + i, column=j, padx=5, pady=5, sticky="ew")
        channel_labels.append(label)  # Store the reference to the label

# Add base functions on the right side
add_base_functions()

# Run the UI
root.mainloop()