import random
import os

determiners = ["the", "a", "an", "this", "that", "each", "every"]

prepositions = [
    "in", "on", "at", "to", "from", "by", "with", "under", "over", 
    "into", "onto", "near", "past", "behind", "beside", "across", 
    "through", "along", "around", "upon"
]

adjectives = [
    # Colors & Simple Visuals
    "red", "blue", "green", "yellow", "black", "white", "brown", "gray", "dark", 
    "bright", "pale", "clear", "golden", "silver", "amber", "bronze", "purple", 
    "orange", "pink", "crimson", "scarlet", "dull", "shiny",
    # Sizes & Shapes
    "big", "small", "tall", "short", "long", "wide", "narrow", "deep", "shallow", 
    "thick", "thin", "heavy", "light", "round", "flat", "curved", "straight", 
    "broad", "tiny", "huge", "vast", "steep",
    # Touch, Temperature & Materials
    "hard", "soft", "warm", "cold", "hot", "cool", "dry", "wet", "rough", "smooth", 
    "sharp", "blunt", "clean", "dirty", "fresh", "sweet", "bitter", "sour", "wooden", 
    "stony", "iron", "clay", "glass", "silk", "wool", "sand", "muddy", "frozen",
    # Basic Qualities
    "good", "bad", "new", "old", "young", "rich", "poor", "strong", "weak", "fast", 
    "slow", "calm", "wild", "tame", "quiet", "loud", "safe", "brave", "wise", "kind", 
    "proud", "busy", "tired", "sleepy", "hungry", "thirsty", "healthy", "sick", "free", 
    "true", "plain", "fine", "fair", "neat", "sturdy", "fragile", "solid", "hollow", 
    "empty", "full", "heavy", "light", "pure", "lost", "found", "dead", "alive"
]

nouns = [
    # Animals (100)
    "dog", "cat", "fox", "wolf", "bear", "lion", "tiger", "deer", "rabbit", "hare", 
    "horse", "colt", "mare", "cow", "bull", "calf", "sheep", "ram", "lamb", "goat", 
    "pig", "boar", "bird", "eagle", "hawk", "owl", "falcon", "crow", "raven", "sparrow", 
    "robin", "swallow", "swan", "goose", "duck", "heron", "crane", "gull", "dove", "pigeon", 
    "fish", "trout", "salmon", "carp", "pike", "bass", "shark", "whale", "seal", "otter", 
    "beaver", "badger", "skunk", "mole", "mouse", "rat", "squirrel", "frog", "toad", "snake", 
    "viper", "python", "lizard", "turtle", "crab", "lobster", "snail", "slug", "worm", "bee", 
    "wasp", "ant", "moth", "beetle", "spider", "donkey", "mule", "camel", "elk", "moose", 
    "bison", "cheetah", "leopard", "panther", "rhino", "hippo", "zebra", "giraffe", "monkey", 
    "ape", "gorilla", "hyena", "jackal", "dingo", "lynx", "bobcat", "falcon", "quail", "turkey", 
    # People & Roles (80)
    "man", "woman", "boy", "girl", "child", "baby", "person", "friend", "guest", "neighbor", 
    "stranger", "king", "queen", "prince", "princess", "lord", "lady", "knight", "squire", 
    "guard", "soldier", "warrior", "archer", "scout", "hunter", "trapper", "farmer", "plowman", 
    "shepherd", "herder", "fisher", "sailor", "captain", "pilot", "baker", "miller", "cook", 
    "brewer", "butcher", "tailor", "weaver", "spinner", "mason", "carpenter", "joiner", 
    "smith", "potter", "shoemaker", "cobbler", "tanner", "dyer", "miner", "forester", "woodman", 
    "healer", "doctor", "nurse", "scholar", "scribe", "teacher", "student", "pupil", "monk", 
    "priest", "judge", "warden", "jailer", "merchant", "trader", "peddler", "innkeeper", 
    "servant", "maid", "porter", "messenger", "runner", "traveler", "pilgrim", "wanderer", "beggar",
    # Places & Terrain (80)
    "forest", "wood", "grove", "jungle", "meadow", "field", "pasture", "prairie", "plain", 
    "hill", "mountain", "ridge", "peak", "cliff", "bluff", "rock", "crag", "valley", "vale", 
    "glen", "canyon", "gorge", "cave", "cavern", "grotto", "hole", "pit", "river", "stream", 
    "brook", "creek", "rill", "lake", "pond", "pool", "puddle", "marsh", "swamp", "bog", 
    "fen", "sea", "ocean", "bay", "cove", "gulf", "harbor", "port", "shore", "coast", 
    "beach", "bank", "island", "isle", "reef", "desert", "dune", "waste", "road", "path", 
    "track", "trail", "way", "street", "lane", "alley", "bridge", "ford", "cross", "town", 
    "city", "village", "hamlet", "camp", "fort", "castle", "tower", "wall", "gate", "fence",
    # Buildings & Household (80)
    "house", "home", "cottage", "cabin", "hut", "shack", "lodge", "palace", "hall", "manor", 
    "barn", "stable", "shed", "sty", "coop", "pen", "mill", "forge", "shop", "store", 
    "market", "inn", "tavern", "room", "chamber", "cell", "kitchen", "pantry", "cellar", "attic", 
    "loft", "roof", "chimney", "hearth", "floor", "door", "window", "shutter", "latch", "lock", 
    "key", "table", "board", "bench", "chair", "stool", "seat", "bed", "couch", "chest", 
    "box", "trunk", "cask", "barrel", "tub", "bucket", "pail", "pot", "pan", "kettle", 
    "bowl", "dish", "plate", "cup", "mug", "glass", "jug", "pitcher", "jar", "bottle", 
    "spoon", "fork", "knife", "blade", "lamp", "lantern", "candle", "torch", "broom", "brush",
    # Food, Plants & Nature (80)
    "tree", "oak", "pine", "fir", "cedar", "elm", "ash", "birch", "maple", "willow", 
    "bush", "shrub", "hedge", "vine", "brier", "fern", "moss", "reed", "rush", "grass", 
    "clover", "weed", "herb", "flower", "rose", "lily", "daisy", "leaf", "bud", "stem", 
    "stalk", "twig", "branch", "bough", "root", "bark", "trunk", "seed", "grain", "wheat", 
    "barley", "oats", "rye", "corn", "rice", "straw", "hay", "fruit", "apple", "pear", 
    "plum", "peach", "cherry", "berry", "grape", "fig", "nut", "acorn", "bread", "loaf", 
    "crust", "flour", "dough", "meal", "meat", "beef", "pork", "fish", "soup", "broth", 
    "milk", "cream", "butter", "cheese", "curd", "egg", "salt", "sugar", "honey", "water",
    # Tools, Materials & Weapons (80)
    "hammer", "mallet", "axe", "hatchet", "adze", "saw", "chisel", "file", "drill", "wedge", 
    "lever", "crowbar", "spade", "shovel", "hoe", "rake", "plow", "scythe", "sickle", "flail", 
    "tongs", "pincers", "shears", "scissors", "needle", "pin", "thread", "yarn", "cord", "rope", 
    "string", "strap", "belt", "chain", "ring", "hook", "peg", "nail", "spike", "bolt", 
    "anvil", "bellows", "furnace", "kiln", "oven", "stone", "rock", "gravel", "sand", "clay", 
    "dirt", "earth", "mud", "dust", "ash", "coal", "charcoal", "iron", "steel", "lead", 
    "tin", "copper", "bronze", "brass", "silver", "gold", "cloth", "linen", "wool", "silk", 
    "leather", "hide", "skin", "pelt", "fur", "horn", "bone", "feather", "scale", "shell",
    # Elements, Sky & Time (80)
    "sun", "moon", "star", "sky", "cloud", "rain", "shower", "drop", "mist", "fog", 
    "haze", "vapor", "frost", "ice", "rime", "snow", "flake", "hail", "sleet", "dew", 
    "wind", "breeze", "draft", "gust", "gale", "storm", "tempest", "thunder", "lightning", "flash", 
    "fire", "flame", "blaze", "spark", "ember", "cinder", "smoke", "soot", "light", "beam", 
    "ray", "glow", "gleam", "glare", "shade", "shadow", "dark", "gloom", "day", "dawn", 
    "morn", "noon", "dusk", "eve", "night", "hour", "time", "season", "spring", "summer", 
    "autumn", "fall", "winter", "year", "wave", "tide", "foam", "spray", "current", "eddy", 
    "sound", "noise", "cry", "call", "voice", "step", "track", "trace", "mark", "sign"
]

verbs = [
    # Perception, Mind & Voice (60)
    "sees", "views", "spots", "notices", "watches", "observes", "tracks", "hears", "hearkens", 
    "listens", "feels", "senses", "smells", "scents", "tastes", "knows", "minds", "thinks", 
    "deems", "fears", "dreads", "hopes", "wishes", "wants", "needs", "loves", "likes", 
    "hates", "calls", "names", "cries", "shouts", "yells", "screams", "howls", "barks", 
    "roars", "growls", "hisses", "chirps", "sings", "hums", "chants", "tells", "says", 
    "speaks", "talks", "asks", "begs", "prays", "warns", "greets", "bids", "thanks", 
    "praises", "blames", "chides", "scolds", "mock", "tricks",
    # Motion & Travel (60)
    "walks", "steps", "paces", "strides", "treads", "runs", "sprints", "races", "hurries", 
    "rushes", "dashes", "flees", "escapes", "shuns", "avoids", "dodges", "leaps", "bounds", 
    "jumps", "springs", "hops", "skips", "climbs", "scales", "creeps", "crawls", "slides", 
    "slips", "glides", "strolls", "wanders", "roams", "strays", "marches", "rides", "drives", 
    "leads", "guides", "follows", "pursues", "chases", "hunts", "tracks", "trails", "swims", 
    "dives", "plunges", "wades", "floats", "drifts", "rows", "paddles", "sails", "steers", 
    "flies", "soars", "flutters", "swoops", "hovers", "perches",
    # Actions, Hands & Craft (60)
    "holds", "grasps", "grips", "clasps", "clutches", "takes", "seizes", "catches", "traps", 
    "snatches", "drops", "sheds", "casts", "throws", "hurls", "flings", "pitches", "tosses", 
    "slings", "lifts", "raises", "heaves", "hoists", "carries", "bears", "totes", "brings", 
    "fetches", "draws", "pulls", "drags", "hauls", "tows", "tugs", "pushes", "shoves", 
    "thrusts", "presses", "squeezes", "pinches", "strikes", "hits", "beats", "taps", "knocks", 
    "raps", "slaps", "smites", "pounds", "hammers", "cuts", "chops", "hews", "slices", 
    "slits", "splits", "hacks", "carves", "chips", "whittles",
    # Household, Building & Care (60)
    "makes", "shapes", "forms", "molds", "builds", "rears", "frames", "fits", "joins", 
    "binds", "ties", "knots", "fastens", "pins", "nails", "glues", "sews", "stitches", 
    "hems", "mends", "patches", "darns", "knits", "weaves", "spins", "twists", "braids", 
    "plaits", "cleans", "washes", "scours", "scrubs", "wipes", "rubs", "polishes", "brushes", 
    "sweeps", "dusts", "scrapes", "cooks", "bakes", "roasts", "boils", "stews", "fries", 
    "brews", "churns", "kneads", "grinds", "pounds", "feeds", "serves", "tends", "nurses", 
    "heals", "cures", "helps", "aids", "shields", "guards",
    # State, Nature & Changes (60)
    "sits", "stands", "rests", "lies", "kneels", "bows", "bends", "leans", "slumps", 
    "sleeps", "slumbers", "dozes", "wakes", "rouses", "rises", "falls", "drops", "sinks", 
    "drowns", "dies", "lives", "thrives", "grows", "sprouts", "shoots", "blooms", "ripens", 
    "yields", "bears", "withers", "fades", "dies", "rots", "decays", "burns", "blazes", 
    "glows", "shines", "flashes", "gleams", "sparks", "smokes", "cools", "chills", "warms", 
    "heats", "freezes", "thaws", "melts", "dries", "wets", "damps", "soaks", "steeps", 
    "opens", "closes", "shuts", "bars", "bolts", "locks"
]

linking_verbs = ["is", "seems", "looks", "feels", "tastes", "smells", "becomes", "stays", "remains"]

# Sets to guarantee every word appears
unused_nouns = set(nouns)
unused_verbs = set(verbs)
unused_adjectives = set(adjectives)

sentences = []

def pick(pool, unused_set):
    if unused_set:
        return unused_set.pop()
    return random.choice(pool)

# Generate simple sentences until all 1,000+ words are consumed
while unused_nouns or unused_verbs or unused_adjectives or len(sentences) < 650:
    det1 = random.choice(determiners)
    det2 = random.choice(determiners)
    prep = random.choice(prepositions)

    # 4 Simple Grammatical Patterns (Max 1 Adjective, No Adverbs)
    mode = random.randint(1, 4)

    if mode == 1:
        # Pattern 1: [det] [adj] [noun] [verb] [det] [noun]  (1 adj on subject)
        adj = pick(adjectives, unused_adjectives)
        n1  = pick(nouns, unused_nouns)
        v   = pick(verbs, unused_verbs)
        n2  = pick(nouns, unused_nouns)
        s = f"{det1} {adj} {n1} {v} {det2} {n2}"

    elif mode == 2:
        # Pattern 2: [det] [noun] [verb] [det] [adj] [noun]  (1 adj on object)
        n1  = pick(nouns, unused_nouns)
        v   = pick(verbs, unused_verbs)
        adj = pick(adjectives, unused_adjectives)
        n2  = pick(nouns, unused_nouns)
        s = f"{det1} {n1} {v} {det2} {adj} {n2}"

    elif mode == 3:
        # Pattern 3: [det] [noun] [verb] [prep] [det] [noun] (0 adj)
        n1 = pick(nouns, unused_nouns)
        v  = pick(verbs, unused_verbs)
        n2 = pick(nouns, unused_nouns)
        s = f"{det1} {n1} {v} {prep} {det2} {n2}"

    else:
        # Pattern 4: [det] [noun] [linking_verb] [adj]        (1 adj complement)
        n1 = pick(nouns, unused_nouns)
        lv = random.choice(linking_verbs)
        adj = pick(adjectives, unused_adjectives)
        s = f"{det1} {n1} {lv} {adj}"

    sentences.append(s)

os.makedirs("data", exist_ok=True)
with open("data/dataset_generated.txt", "w") as f:
    f.write("\n".join(sentences) + "\n")

all_words = set()
total_tokens = 0
for s in sentences:
    words = s.split()
    total_tokens += len(words)
    all_words.update(words)

print(f"[Done] Generated data/dataset.txt:")
print(f"  -> Total sentences    : {len(sentences)}")
print(f"  -> Total words (tokens): {total_tokens}")
print(f"  -> UNIQUE words        : {len(all_words)}")
