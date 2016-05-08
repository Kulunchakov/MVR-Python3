class Model:
    def __init__(self, handle):
        # name of a model
        self.handle = handle

    def __repr__(self):
        return self.handle


    def __hash__(self):
        return hash(self.handle)

    def __eq__(self, other):
        if not isinstance(other, Model):
            return NotImplemented
        return self.handle == other.handle