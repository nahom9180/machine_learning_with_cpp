import sys
import re
import os

def clean_text(raw_text: str):
    # 1. Lowercase everything
    text = raw_text.lower()

    # 2. Replace sentence-ending punctuation with newlines
    # (treats periods, exclamation marks, question marks, and semicolons as sentence breaks)
    text = re.sub(r'[.!?;\n]+', '\n', text)

    # 3. Remove everything except lowercase letters (a-z) and newlines/spaces
    text = re.sub(r'[^a-z\n\s]', ' ', text)

    # 4. Clean up whitespace line by line
    clean_lines = []
    unique_words = set()
    total_tokens = 0

    for line in text.split('\n'):
        words = line.split() # Automatically strips extra spaces/tabs
        # Only keep lines that have at least 2 words (filters out single-character garbage)
        if len(words) >= 2:
            clean_lines.append(" ".join(words))
            total_tokens += len(words)
            unique_words.update(words)

    result_text = "\n".join(clean_lines) + "\n"
    return result_text, len(clean_lines), total_tokens, len(unique_words)


def main():
    # Default file paths
    input_path = sys.argv[1] if len(sys.argv) > 1 else "data/raw.txt"
    output_path = sys.argv[2] if len(sys.argv) > 2 else "data/dataset_cleaned.txt"

    if not os.path.exists(input_path):
        print(f"[Error] Input file '{input_path}' not found!")
        print(f"Usage: python3 tools/clean_text.py [input_file] [output_file]")
        print(f"Or create '{input_path}' and paste your raw text into it.")
        sys.exit(1)

    print(f"[Processor] Reading raw text from: {input_path}")
    with open(input_path, "r", encoding="utf-8", errors="ignore") as f:
        raw = f.read()

    cleaned, num_sentences, total_words, unique_words = clean_text(raw)

    os.makedirs(os.path.dirname(output_path) or ".", exist_ok=True)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(cleaned)

    print("\n" + "=" * 50)
    print("           TEXT CLEANING SUMMARY              ")
    print("=" * 50)
    print(f"  • Sentences created : {num_sentences:,}")
    print(f"  • Total words       : {total_words:,}")
    print(f"  • UNIQUE words      : {unique_words:,}")
    print(f"  • Output written to : {output_path}")
    print("=" * 50)

    if unique_words >= 2000:
        print("✔ Success! You have reached your 2,000+ unique word target!")
    else:
        print(f"ℹ Current vocabulary: {unique_words}/2000 words. (Add more raw text to reach 2,000).")
    print()

if __name__ == "__main__":
    main()
