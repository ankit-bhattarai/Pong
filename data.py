import random

default_titles = ["y1", "y1dot", "y2", "y2dot", "bx", "bxdot", "by", "bydot", "r1", "r2"]

"""
This list indicates what the values in the stream of text from the game mean.
Slider 1 is the left slider, Slider 2 is the right slider.
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
        """Control left and right refer to the control objects for the left and right sliders"""
        self.control_left = control_left
        self.control_right = control_right

    def output_message(self):
        """Instructions to send to the sliders is converted into a string before being sent"""
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
        """Updates the state of the board using the stream of text from the game"""
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
        x: int
            The x position of the slider
        y_height: int
            The height of the slider
        """
        self.side = side
        self.state_board = state_board
        self.x = x
        self.y_height = y_height

    @staticmethod
    def get_sign(number):
        """Returns the text representation of a number's sign"""
        if number >= 0:
            return "+"
        return "-"
    
    def string_response(self):
        response = self.response()
        return f"{self.get_sign(response)}{abs(response)}"
    
    def y(self):
        """Returns the y position of this slider"""
        if self.side == -1:
            return self.state_board.y1
        return self.state_board.y2
    
    def ydot(self):
        """Returns the y velocity of this slider"""
        if self.side == -1:
            return self.state_board.y1dot
        return self.state_board.y2dot
    
    def reward(self):
        """Returns the reward of this slider"""
        if self.side == -1:
            return self.state_board.reward_1
        return self.state_board.reward_2

    def manual_follow(self):
        """Returns the direction to move the slider in order to precisely follow the ball's movement in the y direction"""
        y = self.y()
        if y > self.state_board.ball_y:
            return -1
        elif y < self.state_board.ball_y:
            return 1
        return 0
    
    def calculate_final_y(self):
        """Calculates the y position of the ball when it reaches the same x position as the slider"""
        ball_x_dot = self.state_board.ball_xdot
        ball_y_dot = self.state_board.ball_ydot
        ball_x = self.state_board.ball_x
        ball_y = self.state_board.ball_y

        # Edge Cases
        if (ball_x_dot > 0 and self.side == -1) or (ball_x_dot < 0 and self.side == 1):
            # If the ball is moving away from the slider, return the middle y value to prevent slider moving unnecessarily
            return self.y_height / 2
        if ball_x_dot == 0:
            # If the ball is not moving in the x direction, it will never reach the slider, so return the middle y value to prevent slider moving unnecessarily
            return self.y_height / 2
        if ball_y_dot == 0: # zero y velocity means that the y value wont change
            return ball_y
        x = self.x


        time_for_ball_to_hit_slider = (x - ball_x) / ball_x_dot
        # Calculates the time for the ball to reach the slider based on the x position of the slider and the ball as well as the x velocity of the ball
        # If ball going to right, x > bx and bx_dot > 0
        # If ball going to left, x < bx and bx_dot < 0
        # In both cases time is going to be positive
        
        # In the time the ball reaches the slider, it can go up and down multiple times, ending up at the same y coordinate
        # We are only interested in the final y position so can remove the effect of these trips where the ball bounces between the bottom and top of the board
        time_ball_bottom_top = self.y_height / abs(ball_y_dot) # Time for the ball to go from the bottom to the top
        # by_dot has an abs as it can be both positive and negative
        time_ball_full_trip = 2 * time_ball_bottom_top # Time for the ball to go from the bottom of the board to the top & back down
        # Going up and down several times basically cancels out the net y position

        time_left = time_for_ball_to_hit_slider % time_ball_full_trip
        # In this time left, the ball can only go up and down by an amount less than the full trip
        # This is what we are interested in to evaluate the final y position of the ball
        # There are 2 cases each for when the ball is going up and when the ball is going down
        if ball_y_dot > 0: # ball is going up
            y_gap_from_top = self.y_height - ball_y
            time_for_y_to_reach_top = y_gap_from_top / ball_y_dot
            if time_for_y_to_reach_top >= time_left: # Ball doesn't have enough time to reach the top while y is increasing
                # Final y decided based on current y, y velocity and time left
                y_ball_final = ball_y + ball_y_dot * time_left
            else: # Ball has enough time to go up and to max y and come back down
                time_to_go_down_from_top = time_left - time_for_y_to_reach_top # How much time is left after the ball reaches the top
                # Final y decided based on max y, y velocity and time left as it is going down, hence the negative sign for y velocity
                y_ball_final = self.y_height - ball_y_dot * time_to_go_down_from_top

        else: # y decreasing, ball going down
            y_drop_to_bottom = - ball_y # this value will be negative
            time_for_y_drop_to_bottom = y_drop_to_bottom / ball_y_dot # As both values are negative, this will be positive
            if time_for_y_drop_to_bottom >= time_left: #  Ball doesn't have enough time to reach the bottom while y is decreasing
                y_decrease = ball_y_dot * time_left # This value will also be negative
                y_ball_final = ball_y + y_decrease # As y_decrease is negative, + is the right sign
            else: # Ball has enough time to go down and come back up
                time_to_go_up = time_left - time_for_y_drop_to_bottom
                # Final y decided based on bottom y, y velocity and time left as it is going up, hence the negative sign for y velocity
                y_up_from_bottom =  -1 * ball_y_dot * time_to_go_up
                y_ball_final = y_up_from_bottom
        return y_ball_final

    def calculate_final_y_control(self):
        """Returns the direction to move the slider in order to reach the y position given by estimate_final_y"""
        final_y = self.calculate_final_y()
        y = self.y()
        if y > final_y:
            return -1
        if y < final_y:
            return 1
        return 0


    def response(self):
        #return self.manual_follow()
        return self.calculate_final_y_control()

    





def random_message():
    """Generates a random instruction to send to the sliders in the game
    Message of format "(sign)(number)(sign)(number)" e.g. "+1-1" or "+0-1"""
    number1 = random.randint(-1, 1)
    number2 = random.randint(-1, 1)
    text = f"{Control.get_sign(number1)}{abs(number1)}{Control.get_sign(number2)}{abs(number2)}"
    return text
