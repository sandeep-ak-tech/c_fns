import os

# ================== CONFIGURATION ==================
folder_path = "."   # Current folder (where the script is located)
# If you want to target a specific folder, change it like:
# folder_path = r"C:\Users\YourName\Downloads\MyFiles"

# Common patterns to remove from the beginning:
# Examples: "01_file.txt" → "file.txt"
#           "001 - photo.jpg" → "photo.jpg"
#           "12_report.pdf" → "report.pdf"

remove_patterns = ["_", "-", " ", "."]  # separators after the number

dry_run = False     # Set to True first to see what will be renamed (safe mode)

# ===================================================

def remove_number_prefix(filename):
    """Remove leading numbers and common separators from filename"""
    name, ext = os.path.splitext(filename)
    
    # Strip leading whitespace
    cleaned = name.strip()
    
    # Remove leading numbers
    i = 0
    while i < len(cleaned) and cleaned[i].isdigit():
        i += 1
    
    # If we found numbers at the start
    if i > 0:
        # Remove the numbers and any following separator
        remaining = cleaned[i:].strip()
        
        # Remove common separators at the beginning
        for pattern in remove_patterns:
            if remaining.startswith(pattern):
                remaining = remaining[len(pattern):].strip()
        
        new_name = remaining + ext
        return new_name.strip()
    
    return filename  # No change if no leading number


def main():
    print(f"Scanning folder: {os.path.abspath(folder_path)}\n")
    
    renamed_count = 0
    skipped_count = 0
    
    for filename in os.listdir(folder_path):
        old_path = os.path.join(folder_path, filename)
        
        # Skip directories and hidden files
        if os.path.isdir(old_path) or filename.startswith('.'):
            continue
        
        new_filename = remove_number_prefix(filename)
        
        # If name changed
        if new_filename != filename:
            new_path = os.path.join(folder_path, new_filename)
            
            # Check if target file already exists
            if os.path.exists(new_path):
                print(f"⚠️  Skipped (file already exists): {new_filename}")
                skipped_count += 1
                continue
            
            try:
                if dry_run:
                    print(f"[DRY RUN] Would rename: {filename}  →  {new_filename}")
                else:
                    os.rename(old_path, new_path)
                    print(f"✅ Renamed: {filename}  →  {new_filename}")
                renamed_count += 1
            except Exception as e:
                print(f"❌ Error renaming {filename}: {e}")
                skipped_count += 1
        else:
            # No number prefix found
            pass  # silently skip

    print("\n" + "="*50)
    if dry_run:
        print("DRY RUN COMPLETED - No files were actually renamed")
    print(f"Total files processed : {renamed_count + skipped_count}")
    print(f"Successfully renamed   : {renamed_count}")
    print(f"Skipped                : {skipped_count}")
    print("="*50)

if __name__ == "__main__":
    # Safety prompt
    print("This script will remove leading numbers from all filenames in the current folder.")
    print("Example: '01_report.pdf' → 'report.pdf' | '002 - photo.jpg' → 'photo.jpg'\n")
    
    confirm = input("Do you want to continue? (y/n): ").strip().lower()
    if confirm == 'y' or confirm == 'yes':
        main()
    else:
        print("Operation cancelled by user.")