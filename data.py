import random

default_titles = ["y1", "y1dot", "y2", "y2dot", "bx", "bxdot", "by", "bydot", "r1", "r2"]

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





class State:
    def __init__(self):
        self.y1 = None
        self.y1dot = None
        self.y2 = None
        self.y2dot = None
        self.ball_x = None
        self.ball_y = None
        self.ball_xdot = None
        self.ball_ydot = None
        self.reward_1 = None
        self.reward_2 = None
        self.control_left = None
        self.control_right = None

    def get_control_objects(self, control_left, control_right):
        self.control_left = control_left
        self.control_right = control_right

    def output_message(self):
        message1 = self.control_left.string_response()
        message2 = self.control_right.string_response()
        return message1 + message2

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
        self.y1 = dictionary["y1"]
        self.y2 = dictionary["y2"]
        self.y1dot = dictionary["y1dot"]
        self.y2dot = dictionary["y2dot"]
        self.ball_x = dictionary["bx"]
        self.ball_y = dictionary["by"]
        self.ball_xdot = dictionary["bxdot"]
        self.ball_ydot = dictionary["bydot"]
        self.reward_1 = dictionary["r1"]
        self.reward_2 = dictionary["r2"]

class Control:
    def __init__(self, side, state_board, x, y_height):
        """

        Parameters
        ----------
        side:int
            -1 for left slider, 1 for right slider
        state_board: State
            An object of the state class
        """
        self.side = side
        self.state_board = state_board
        self.x = x
        self.y_height = y_height
        self.need_to_flip = False

    def y(self):
        if self.side == -1:
            return self.state_board.y1
        return self.state_board.y2
    def ydot(self):
        if self.side == -1:
            return self.state_board.y1dot
        return self.state_board.y2dot

    def manual_follow(self):
        y = self.y()
        if y > self.state_board.ball_y:
            return -1
        elif y < self.state_board.ball_y:
            return 1
        return 0

    def estimate_final_y(self):
        bx_dot = self.state_board.ball_xdot
        if bx_dot == -1 * self.side:
            return self.y_height / 2
        if bx_dot == 0:
            return self.y_height / 2
        by_dot = self.state_board.ball_ydot

        bx = self.state_board.ball_x
        by = self.state_board.ball_y
        if by_dot == 0: # zero y velocity means that the y value wont change
            return by
        x = self.x #1425
        time_for_ball_to_hit_slider = (x - bx) / bx_dot
        # If ball going to right, x > bx and bx_dot > 0
        # If ball going to left, x < bx and bx_dot < 0
        # In both cases time is going to be positive
        time_ball_bottom_top = self.y_height / abs(by_dot) # Time for the ball to go from the bottom to the top
        # by_dot has an abs as it can be both positive and negative
        time_ball_full_trip = 2 * time_ball_bottom_top # Time for the ball to go from the bottom to the top & back down
        # Going up and down several times basically cancels out the net y position
        time_left = time_for_ball_to_hit_slider % time_ball_full_trip
        # In this time left, the ball can only go up and down by an amount less than the full trip
        if by_dot > 0: # y increasing
            y_to_increase = self.y_height - by
            time_for_y_to_increase = y_to_increase / by_dot
            if time_for_y_to_increase >= time_left: # Ball doesn't have enough time to increase in y
                y_increase = by_dot * time_left
                y_ball_final = by + y_increase
            else: # Ball has enough time to go up and come back down
                time_to_go_down = time_left - time_for_y_to_increase
                y_down_from_top = by_dot * time_to_go_down
                y_ball_final = self.y_height - y_down_from_top
        else: # y decreasing
            y_to_decrease = - by # this value will be negative
            time_for_y_to_decrease = y_to_decrease / by_dot # As both values are negative, this will be positive
            if time_for_y_to_decrease >= time_left:
                y_decrease = by_dot * time_left # This value will also be negative
                y_ball_final = by + y_decrease # As y_decrease is negative, + is the right sign
            else: # Ball has enough time to go down and come back up
                time_to_go_up = time_left - time_for_y_to_decrease
                y_up_from_bottom = abs(by_dot) * time_to_go_up
                y_ball_final = y_up_from_bottom
        return y_ball_final

    def exact_y_control(self):
        final_y = self.estimate_final_y()
        y = self.y()
        if y > final_y:
            return -1
        if y < final_y:
            return 1
        return 0



    def response(self):
        #return self.manual_follow()
        return self.exact_y_control()
    @staticmethod
    def get_sign(number):
        """Returns the text representation of a numbers sign"""
        if number >= 0:
            return "+"
        return "-"
    def string_response(self):
        response = self.response()
        return f"{self.get_sign(response)}{abs(response)}"

    def time_to_impact(self):
        if self.side == 1:
            x = 75
        else:
            x = 1425
        ball_x = self.state_board.ball_x
        ball_xdot = self.state_board.ball_xdot





def random_message():
    """Generates a random instruction to send to the sliders in the game
    Message of format "(sign)(number)(sign)(number)" e.g. "+1-1" or "+0-1"""
    number1 = random.randint(-1, 1)
    number2 = random.randint(-1, 1)
    text = f"{Control.get_sign(number1)}{abs(number1)}{Control.get_sign(number2)}{abs(number2)}"
    return text