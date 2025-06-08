#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#define MAX_LENGTH 500

// ANSI color codes
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

// Function prototypes
void displayBanner();
void displayYoutubePrompt();
int crackHash(const char *hashFile, const char *wordlist);
int crackZip(const char *zipFile, const char *wordlist);
void displayPassword(const char *password);
void showProgressSpinner();
int runCommand(const char *cmd, char *result, int capture_output);
int fileExists(const char *path);
double estimateFileSize(int minLen, int maxLen, int charSetSize);
int generateCustomWordlist(const char *filename, int minLen, int maxLen, 
                           int upper, int lower, int digit, int special);

int main() {
    displayBanner();
    
    int choice;
    char path[MAX_LENGTH] = {0};
    char wordlist[MAX_LENGTH] = {0};
    int found = 0;

    printf(CYAN "\nDo you want to crack Hash or Encrypted file? (1=Hash, 2=Encrypted File): " RESET);
    if (scanf("%d", &choice) != 1) {
        printf(RED "\nInvalid input!\n" RESET);
        return 1;
    }
    getchar(); // Consume newline

    printf(CYAN "\nProvide the file path: " RESET);
    fgets(path, MAX_LENGTH, stdin);
    path[strcspn(path, "\n")] = '\0'; // Remove newline

    if (!fileExists(path)) {
        printf(RED "\nError: File does not exist!\n" RESET);
        return 1;
    }

    int useOwnWordlist;
    printf(CYAN "\nDo you want to use your own wordlist? (1=Yes, 0=No): " RESET);
    if (scanf("%d", &useOwnWordlist) != 1) {
        printf(RED "\nInvalid input!\n" RESET);
        return 1;
    }
    getchar();

    if (useOwnWordlist) {
        printf(CYAN "Provide the path of the wordlist: " RESET);
        fgets(wordlist, MAX_LENGTH, stdin);
        wordlist[strcspn(wordlist, "\n")] = '\0';
        
        if (!fileExists(wordlist)) {
            printf(RED "\nError: Wordlist file does not exist!\n" RESET);
            return 1;
        }
    } else {
        printf(YELLOW "\nUsing rockyou.txt wordlist...\n" RESET);
        strcpy(wordlist, "/usr/share/wordlists/rockyou.txt");
        
        if (!fileExists(wordlist)) {
            printf(RED "\nError: rockyou.txt not found! Install kali-linux-default package.\n" RESET);
            return 1;
        }
    }

    // Attempt cracking
    if (choice == 1) {
        found = crackHash(path, wordlist);
    } else if (choice == 2) {
        found = crackZip(path, wordlist);
    } else {
        printf(RED "\nInvalid choice!\n" RESET);
        return 1;
    }

    if (!found) {
        printf(RED "\nPassword not found with the wordlist.\n" RESET);
        int createCustom;
        printf(CYAN "\nDo you want to create a custom wordlist? (1=Yes, 0=No): " RESET);
        if (scanf("%d", &createCustom) != 1) {
            printf(RED "\nInvalid input!\n" RESET);
            return 1;
        }

        if (createCustom) {
            int minLen, maxLen;
            int upper, lower, digit, special;
            
            printf(CYAN "\nMinimum password length: " RESET);
            if (scanf("%d", &minLen) != 1) minLen = 1;
            printf(CYAN "Maximum password length: " RESET);
            if (scanf("%d", &maxLen) != 1) maxLen = 8;
            
            printf(CYAN "\nInclude uppercase letters? (1=Yes, 0=No): " RESET);
            if (scanf("%d", &upper) != 1) upper = 0;
            printf(CYAN "Include lowercase letters? (1=Yes, 0=No): " RESET);
            if (scanf("%d", &lower) != 1) lower = 1;
            printf(CYAN "Include digits? (1=Yes, 0=No): " RESET);
            if (scanf("%d", &digit) != 1) digit = 0;
            printf(CYAN "Include special characters? (1=Yes, 0=No): " RESET);
            if (scanf("%d", &special) != 1) special = 0;
            
            int charSetSize = (upper?26:0) + (lower?26:0) + (digit?10:0) + (special?32:0);
            if (charSetSize == 0) {
                printf(RED "\nError: No character set selected!\n" RESET);
                return 1;
            }
            
            double estSize = estimateFileSize(minLen, maxLen, charSetSize);
            
            printf(YELLOW "\nEstimated wordlist size: %.2f MB\n" RESET, estSize);
            
            if (estSize > 100) {
                printf(RED "Warning: This might take significant time and storage!\n" RESET);
            }
            
            int proceed;
            printf(CYAN "\nDo you want to proceed? (1=Yes, 0=No): " RESET);
            if (scanf("%d", &proceed) != 1) proceed = 0;
            
            if (proceed) {
                strcpy(wordlist, "/tmp/custom_wordlist.txt");
                if (generateCustomWordlist(wordlist, minLen, maxLen, upper, lower, digit, special)) {
                    // Try cracking with custom wordlist
                    if (choice == 1) {
                        found = crackHash(path, wordlist);
                    } else if (choice == 2) {
                        found = crackZip(path, wordlist);
                    }
                } else {
                    printf(RED "\nWordlist generation failed!\n" RESET);
                }
            }
        }
        
        if (!found) {
            printf(RED "\n***************************************************\n");
            printf("Password cracking failed!\n");
            printf("Cracking passwords is not a joke. Sometimes it takes\n");
            printf("ages to crack a password depending on its complexity.\n");
            printf("***************************************************\n\n" RESET);
            displayYoutubePrompt();
        }
    }
    
    return 0;
}

int fileExists(const char *path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void displayBanner() {
    printf(GREEN "\n"
        " █████╗ ███╗   ██╗ ██████╗ ███╗   ███╗██╗████████╗██╗   ██╗\n"
        "██╔══██╗████╗  ██║██╔═══██╗████╗ ████║██║╚══██╔══╝╚██╗ ██╔╝\n"
        "███████║██╔██╗ ██║██║   ██║██╔████╔██║██║   ██║    ╚████╔╝ \n"
        "██╔══██║██║╚██╗██║██║   ██║██║╚██╔╝██║██║   ██║     ╚██╔╝  \n"
        "██║  ██║██║ ╚████║╚██████╔╝██║ ╚═╝ ██║██║   ██║      ██║   \n"
        "╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝     ╚═╝╚═╝   ╚═╝      ╚═╝   \n" RESET);
    
    printf(BLUE "_____________________________________________________\n");
    printf("Created by Anomity\n");
    printf("\nPassword Cracking Tool for Kali Linux\n");
    printf("Supports hash cracking and encrypted file password recovery\n");
    printf("_____________________________________________________\n\n" RESET);
    
    displayYoutubePrompt();
}

void displayYoutubePrompt() {
    printf(CYAN "\nPlease support by subscribing to my YouTube channel:\n");
    printf(MAGENTA "https://www.youtube.com/c/Anomity\n\n" RESET);
}

void displayPassword(const char *password) {
    printf(GREEN "\n***************************************************\n");
    printf(RED "  Password Found: %s\n", password);
    printf(GREEN "***************************************************\n\n" RESET);
    displayYoutubePrompt();
    exit(0);
}

int crackHash(const char *hashFile, const char *wordlist) {
    printf(YELLOW "\n[+] Cracking hash with John the Ripper...\n" RESET);
    
    // Create a temporary file for the hash
    char hashTemp[] = "/tmp/anomity_hash_XXXXXX";
    int fd = mkstemp(hashTemp);
    if (fd == -1) {
        perror("mkstemp");
        return 0;
    }
    close(fd);

    // Command to run John
    char command[MAX_LENGTH * 2];
    snprintf(command, sizeof(command), "john --wordlist=%s --format=raw-md5 %s", wordlist, hashFile);
    
    char result[MAX_LENGTH] = {0};
    if (runCommand(command, result, 1)) {
        // Check for password
        snprintf(command, sizeof(command), "john --show %s", hashTemp);
        if (runCommand(command, result, 1)) {
            char *colon = strchr(result, ':');
            if (colon) {
                char password[100];
                sscanf(colon + 1, "%99[^:]", password);
                displayPassword(password);
                return 1;
            }
        }
    }
    
    printf(RED "No password found in wordlist\n" RESET);
    return 0;
}

int crackZip(const char *zipFile, const char *wordlist) {
    printf(YELLOW "\n[+] Cracking ZIP with fcrackzip...\n" RESET);
    
    char command[MAX_LENGTH * 2];
    char result[MAX_LENGTH] = {0};
    
    // 1. First try with fcrackzip
    snprintf(command, sizeof(command), 
        "fcrackzip -u -D -p '%s' '%s'", 
        wordlist, zipFile);
    
    if (runCommand(command, result, 1)) {
        // Parse fcrackzip output
        char *pw = strstr(result, "PASSWORD FOUND!!!!: pw == ");
        if (pw) {
            char password[100];
            if (sscanf(pw, "PASSWORD FOUND!!!!: pw == %99s", password) == 1) {
                displayPassword(password);
                return 1;
            }
        }
    }

    // 2. If fcrackzip fails, use John the Ripper
    printf(RED "\n[-] fcrackzip failed! Trying John the Ripper...\n" RESET);
    
    char hashFile[] = "/tmp/anomity_hash_XXXXXX";
    int fd = mkstemp(hashFile);
    if (fd == -1) {
        perror("mkstemp");
        return 0;
    }
    close(fd);

    // Generate hash with zip2john
    snprintf(command, sizeof(command), "zip2john '%s' > '%s'", zipFile, hashFile);
    if (!runCommand(command, NULL, 0)) {
        printf(RED "Error: zip2john failed. Install with 'sudo apt install john'\n" RESET);
        return 0;
    }

    // Crack with John
    snprintf(command, sizeof(command), 
        "john --wordlist='%s' '%s'", 
        wordlist, hashFile);
    
    printf(CYAN "\n[+] John the Ripper running...\n" RESET);
    if (runCommand(command, result, 1)) {
        // Check results
        snprintf(command, sizeof(command), "john --show '%s'", hashFile);
        if (runCommand(command, result, 1)) {
            char *colon = strchr(result, ':');
            if (colon) {
                char password[100];
                sscanf(colon + 1, "%99[^:]", password);
                displayPassword(password);
                return 1;
            }
        }
    }
    
    printf(RED "\n[-] John failed. Try manual cracking:\n" RESET);
    printf("  zip2john %s > hashes.txt\n", zipFile);
    printf("  hashcat -m 17200 -a 0 hashes.txt %s\n", wordlist);
    return 0;
}

int runCommand(const char *cmd, char *result, int capture_output) {
    int status;
    pid_t pid = fork();
    
    if (pid == 0) {  // Child process
        if (capture_output) {
            int fd = open("/tmp/anomity_out", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        execl("/bin/sh", "sh", "-c", cmd, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {  // Parent process
        int status;
        waitpid(pid, &status, 0);
        
        if (capture_output) {
            FILE *fp = fopen("/tmp/anomity_out", "r");
            if (fp) {
                if (result) {
                    fread(result, 1, MAX_LENGTH-1, fp);
                    result[MAX_LENGTH-1] = '\0';
                }
                fclose(fp);
                remove("/tmp/anomity_out");
            }
        }
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    } else {
        perror("fork");
        return 0;
    }
}

void showProgressSpinner() {
    static int counter = 0;
    const char *spinner = "|/-\\";
    printf(BLUE "[%c]" CYAN " Cracking...\r" RESET, spinner[counter % 4]);
    fflush(stdout);
    counter++;
}

double estimateFileSize(int minLen, int maxLen, int charSetSize) {
    double totalSize = 0;
    for (int len = minLen; len <= maxLen; len++) {
        totalSize += pow(charSetSize, len) * (len + 1); // +1 for newline
    }
    return totalSize / (1024 * 1024); // Convert to MB
}

int generateCustomWordlist(const char *filename, int minLen, int maxLen, 
                           int upper, int lower, int digit, int special) {
    printf(YELLOW "\n[+] Generating custom wordlist...\n" RESET);
    
    char charset[100] = "";
    if (lower) strcat(charset, "abcdefghijklmnopqrstuvwxyz");
    if (upper) strcat(charset, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (digit) strcat(charset, "0123456789");
    if (special) strcat(charset, "!@#$%^&*()_+-=[]{}|;:,.<>?");
    
    int charsetLen = strlen(charset);
    if (charsetLen == 0) {
        printf(RED "Error: No character set defined!\n" RESET);
        return 0;
    }
    
    // Prevent excessively large generations
    if (maxLen > 6) {
        printf(YELLOW "Warning: Reducing max length to 6 for practical reasons\n" RESET);
        maxLen = 6;
    }
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error creating wordlist file");
        return 0;
    }
    
    // Generate passwords recursively
    long total = 0;
    for (int len = minLen; len <= maxLen; len++) {
        char *buf = calloc(len+1, sizeof(char));
        if (!buf) {
            perror("Memory allocation failed");
            fclose(fp);
            return 0;
        }
        
        // Initialize buffer
        for (int i = 0; i < len; i++) {
            buf[i] = charset[0];
        }
        
        long count = 0;
        long totalForLen = (long)pow(charsetLen, len);
        
        do {
            // Write current password
            fprintf(fp, "%s\n", buf);
            count++;
            
            // Show progress every 1000 entries
            if (count % 1000 == 0) {
                float progress = (float)count / totalForLen;
                printf(BLUE "[%.1f%%] Generated %ld/%ld for length %d\r" RESET, progress*100, count, totalForLen, len);
                fflush(stdout);
            }
            
            // Generate next password
            int pos = len - 1;
            while (pos >= 0) {
                char *p = strchr(charset, buf[pos]);
                if (p && (p - charset < charsetLen - 1)) {
                    buf[pos] = charset[p - charset + 1];
                    break;
                } else {
                    buf[pos] = charset[0];
                    pos--;
                }
            }
            if (pos < 0) break;
        } while (1);
        
        total += count;
        free(buf);
        printf("\nGenerated %ld passwords of length %d\n", count, len);
    }
    
    fclose(fp);
    printf(YELLOW "Wordlist generated at %s (%ld entries)\n" RESET, filename, total);
    return 1;
}
