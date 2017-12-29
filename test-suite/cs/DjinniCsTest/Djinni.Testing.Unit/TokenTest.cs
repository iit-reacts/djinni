using Djinni.TestSuite;
using NUnit.Framework;

namespace Djinni.Testing.Unit
{
    [TestFixture]
    public class TokenTest
    {
        private class CsToken : UserToken
        {
            public override string Whoami()
            {
                return "C#";
            }
        }

        [Test]
        public void TestTokens()
        {
            UserToken token = new CsToken();
            Assert.AreSame(token, TestHelpers.TokenId(token));
        }

        [Test]
        public void TestNullToken()
        {
            Assert.AreSame(null, TestHelpers.TokenId(null));
        }

        [Test]
        public void TestCppToken()
        {
            UserToken token = TestHelpers.CreateCppToken();
            Assert.AreSame(token, TestHelpers.TokenId(token));
            TestHelpers.CheckCppToken(token);
            // token = null;
            // System.gc();
            // System.runFinalization();
        }
    }
}