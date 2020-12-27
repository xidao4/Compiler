import os


def analyzeOne():
    with open(f'{pathBase}/{trueAnswer}') as f:
        trueAnswerList = [i for i in f.read().strip().replace('Error Type ', 'Error type ').split('\n') if i]
    with open(f'{pathBase}/{yourAnswer}') as f:
        yourAnswerList = [i for i in f.read().strip().split('\n') if i]
    trueAnswerDict = dict()
    yourAnswerDict = dict()
    for i in range(len(trueAnswerList)):
        trueAnswerDict[trueAnswerList[i].split()[2] + '&' + trueAnswerList[i].split()[5]] = i
    for i in range(len(yourAnswerList)):
        yourAnswerDict[yourAnswerList[i].split()[2] + '&' + yourAnswerList[i].split()[5]] = i
    extraList = [yourAnswerList[yourAnswerDict[i]] for i in yourAnswerDict if i not in trueAnswerDict]
    missingList = [trueAnswerList[trueAnswerDict[i]] for i in trueAnswerDict if i not in yourAnswerDict]

    if extraList or missingList:
        print('\n  your extra output: ')
        [print(i) for i in extraList]
        print('\n  your missing output: ')
        [print(i) for i in missingList]
        input()


if __name__ == "__main__":
    pathBase = './tests/myLab/Tests_2_Advanced'

    li = [i.replace('.cmm', '') for i in os.listdir(pathBase) if i.endswith('.cmm')]
    for i in li:
        print(i)
        trueAnswer = i + '.output'
        yourAnswer = i + '.out.txt'
        try:
            analyzeOne()
        except BaseException as e:
            print(e)
            print(repr(e))
            print('error in file: ' + i)
            print('please input \'c\' to check the details of your answer')
            c = input()
            if c == 'c':
                print(open(f'{pathBase}/{yourAnswer}').read())
                input()
