import os

# ================== CONFIGURATION ==================
# List your filenames here in the exact order you want them numbered
file_list = [
    "code_2d_array_access_using_pointer.c",
    "code_allocate_2d_array_with_malloc.c",
    "code_transpose_of_a_matrix.c",
    "code_mirror_of_a_matrix.c",
    "code_unique_id_generator.c",
    "code_reverse_linked_list.c",
    "code_linked_list_palindrom.c",
    "find_loop_in_linked_list.c",
    "merge_two_sorted_linked_list.c",
    "binary_search.c",
    "find_index_in_rotated_sorted_array.c",
    "code_quick_sort.c",
    "code_square_root_log_fn.c",
    "code_square_root_simple_fn.c",
    "kth_large_quick_srt.c",
    "circular_buffer_non_blocking.c",
    "kth_largest_min_heap.c",
    "malloc_in_c_implementation.c",
    "sets.c",
    # Add more files here in the order you want
]

# Optional: Choose prefix and separator
prefix = ""           # e.g., "file_" or leave empty
separator = "_"       # e.g., "_", "-", " "
start_number = 1      # Start numbering from 1 or 0
padding = 2           # Zero-padding: 01, 02, 03... (use 3 for 001, etc.)

# ===================================================

def rename_files():
    folder = os.path.dirname(os.path.abspath(__file__))  # Same folder as script
    renamed_count = 0

    print(f"Current folder: {folder}\n")

    for i, original_name in enumerate(file_list, start=start_number):
        old_path = os.path.join(folder, original_name)
        
        if not os.path.exists(old_path):
            print(f"❌ File not found: {original_name}")
            continue

        # Get file extension
        name, ext = os.path.splitext(original_name)
        
        # Create new name: prefix + number + separator + original_name
        number_str = f"{i:0{padding}d}"
        new_name = f"{prefix}{number_str}{separator}{name}{ext}"
        
        new_path = os.path.join(folder, new_name)

        # Check if new name already exists
        if os.path.exists(new_path):
            print(f"⚠️  Skipped (already exists): {new_name}")
            continue

        try:
            os.rename(old_path, new_path)
            print(f"✅ Renamed: {original_name}  →  {new_name}")
            renamed_count += 1
        except Exception as e:
            print(f"❌ Error renaming {original_name}: {e}")

    print(f"\n🎉 Done! {renamed_count} file(s) renamed.")

if __name__ == "__main__":
    rename_files()