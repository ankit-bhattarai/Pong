import random

default_titles = ["x1", "x1dot", "x2", "x2dot", "bx", "bxdot", "by", "bydot", "r1", "r2"]

"""
The values in this list correspond to:
1. Slider 1 Position
2. Slider 1 Velocity
3. Slider 2 Position
4. Slider 2 Velocity
5. Ball X position
6. Ball X Velocity
7. Ball Y position
8. Ball Y Velocity
9. Slider 1 Reward
10. Slider 2 Reward
"""

def get_sign(number):
    """Returns the text representation of a numbers sign"""
    if number >= 0:
        return "+"
    return "-"

def random_message():
    """Generates a random instruction to send to the sliders in the game
    Message of format "(sign)(number)(sign)(number)" e.g. "+1-1" or "+0-1"""
    number1 = random.randint(-1, 1)
    number2 = random.randint(-1, 1)
    text = f"{get_sign(number1)}{abs(number1)}{get_sign(number2)}{abs(number2)}"
    return text



class State:
    def __init__(self):
        self.x1 = None
        self.x1dot = None
        self.x2 = None
        self.x2dot = None
        self.ball_x = None
        self.ball_y = None
        self.ball_xdot = None
        self.ball_ydot = None
        self.reward_1 = None
        self.reward_2 = None

    @staticmethod
    def convert_stream_to_list(stream_text):
        """Breaks down a stream of text containing floats seperated by spaces into a list of floats"""
        data_list = [float(i) for i in stream_text.split()]
        return data_list

    @classmethod
    def get_dictionary_from_stream(cls, stream_text, keys=default_titles):
        """Converts the stream into a dictionary with the values of the dictionary being values from the stream"""
        values = cls.convert_stream_to_list(stream_text)
        dictionary = {key: value for key, value in zip(keys, values)}
        return dictionary

    def update_state(self, stream_text, keys=default_titles):
        dictionary = self.get_dictionary_from_stream(stream_text, keys)
        self.x1 = dictionary["x1"]
        self.x2 = dictionary["x2"]
        self.x1dot = dictionary["x1dot"]
        self.x2dot = dictionary["x2dot"]
        self.ball_x = dictionary["bx"]
        self.ball_y = dictionary["by"]
        self.ball_xdot = dictionary["bxdot"]
        self.ball_ydot = dictionary["bydot"]
        self.reward_1 = dictionary["r1"]
        self.reward_2 = dictionary["r2"]
